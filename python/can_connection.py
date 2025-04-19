import can
from dataclasses import dataclass
from typing import Callable


@dataclass
class CanFrame:
    """Custom class for a CAN message."""
    id: int
    is_extended: bool
    dlc: int
    data: list[int]


class CanConnection:
    """Wrapper class for CAN communication.
    
    Args:
        can_interface: Name of the CAN interface.
    """
    def __init__(self, can_interface: str="can0") -> None:
        self._bus: can.Bus = None
        self._notifier: can.Notifier = None
        self._callbacks: Callable = []
        self._ready_to_read: bool = False
        self._connected: bool = False
        if can_interface:
            self._can_interface = can_interface
            self.open(can_interface)

    def open(self, can_interface: str="can0") -> None:
        """Open CAN communication to can_interface.
        
        Args:
            can_interface: Name of the CAN interface.
        """
        try:
            self._bus = can.ThreadSafeBus(
                interface="socketcan",
                channel=can_interface
            )
            self._notifier = can.Notifier(
                self._bus,
                [self._on_message_received],
                timeout=0.1
            )
            self._connected = True
        except Exception as e:
            print("Could not connect, exception: ", e)
    
    def __del__(self):
        self.close()

    def close(self) -> None:
        """Close CAN communication."""
        if not self._connected:
            return
        
        if self._notifier:
            self._notifier.stop()
            self._notifier = None
        if self._bus:
            self._bus.shutdown()
            self._bus = None
        self._connected = False
    
    def add_callback(self, callback: Callable[[CanFrame],None]) -> None:
        """Add callback to call when a new message is received.
        
        Args:
            callback: Function to be executed when a new CAN message is received.
        """
        self._callbacks.append(callback)

    def _on_message_received(self, msg: can.Message) -> None:
        if not self._ready_to_read:
            return
        data = [x for x in msg.data]
        can_frame = CanFrame(
            id=msg.arbitration_id,
            is_extended=msg.is_extended_id,
            dlc=msg.dlc,
            data=data
        )
        for callback in self._callbacks:
            callback(can_frame)
    
    def send(self, frame: CanFrame) -> None:
        """Send a CAN message.
        
        Args:
            frame: CAN message.
        """
        if not self._connected:
            self.open(self._can_interface)
            return
        
        self._bus.send(can.Message(
            arbitration_id=frame.id,
            is_extended_id=frame.is_extended,
            dlc=frame.dlc,
            data=frame.data
        ))

    def ready_to_read(self) -> None:
        """Signal the class it is ready to start reading CAN messages."""
        if not self._connected:
            print("CAN communication not open")
            return
        self._ready_to_read = True
import sys
from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout, QApplication
from PyQt5.QtCore import Qt, QSize

from can_connection import CanFrame, CanConnection


class Window(QWidget):
    """PyQt5 window class."""
    def __init__(self, parent: QWidget=None) -> None:
        super().__init__(parent)
        self._setup_ui()

    def _setup_ui(self) -> None:
        self.labelTitle = QLabel("Value", self)
        self.labelTitle.setStyleSheet("font: 16px")
        self.labelTitle.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.value = QLabel("0", self)
        self.value.setStyleSheet("font: 64px")
        self.value.setAlignment(Qt.AlignmentFlag.AlignCenter)
        
        layout = QVBoxLayout(self)
        layout.addWidget(self.labelTitle)
        layout.addWidget(self.value)
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setLayout(layout)
    
    def sizeHint(self) -> QSize:
        return QSize(150, 80)
    
    def set_value(self, value: int) -> None:
        self.value.setText(f"{value}")


class AppControl(Window):
    def __init__(self, parent: QWidget=None, can_interface: str="can0") -> None:
        super().__init__(parent)
        self.can_comm: CanConnection = CanConnection(can_interface)
        self.can_comm.add_callback(self.on_message_received)
        self.can_comm.ready_to_read()

    def on_message_received(self, message: CanFrame) -> None:
        self.set_value(message.data[0])


if __name__ == "__main__":
    app = QApplication([])
    window = AppControl()
    window.show()
    sys.exit(app.exec())
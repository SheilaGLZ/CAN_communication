import sys
from PyQt5.QtWidgets import (QWidget, QLabel, QSlider, QVBoxLayout,
    QApplication)
from PyQt5.QtCore import Qt, QSize, pyqtSignal, QTimer

from can_connection import CanFrame, CanConnection


class Window(QWidget):
    """PyQt5 window class."""
    value_changed = pyqtSignal(int)

    def __init__(self, parent:QWidget=None) -> None:
        super().__init__(parent)
        self._setup_ui()
        self._connect_slots()
    
    def _setup_ui(self) -> None:
        self.setStyleSheet("font: 16px")
        self.label = QLabel("Value", self)
        self.label.setMinimumWidth(100)
        self.label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.value = QLabel("0", self)
        self.value.setMinimumWidth(100)
        self.value.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.slider = QSlider(Qt.Orientation.Vertical, self)
        self.slider.setMinimumWidth(100)
        self.slider.setRange(0, 100)

        layout = QVBoxLayout(self)
        layout.addWidget(self.label)
        layout.addWidget(self.slider)
        layout.addWidget(self.value)
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setLayout(layout)
    
    def sizeHint(self) -> QSize:
        """Recommended window size."""
        return QSize(100, 300)
    
    def _connect_slots(self) -> None:
        self.slider.sliderMoved.connect(self._on_slider_moved)
    
    def _on_slider_moved(self, position: int) -> None:
        self.value.setText(f"{position}")
        self.value_changed.emit(position)


class AppControl(Window):
    """Can communication + GUI control."""
    def __init__(self, parent:QWidget=None, can_interface:str="can0") -> None:
        super().__init__(parent)
        self.can_comm: CanConnection = CanConnection(can_interface)
        self.value_changed.connect(self.on_value_changed)
        self.can_message = CanFrame(100, False, 1, [0])
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.send_can)
        self.timer.start(100)

    def closeEvent(self, event) -> None:
        """Close window"""
        self.timer.stop()
        event.accept()

    def on_value_changed(self, value: int) -> None:
        """Slider value changed, send new CAN value."""
        self.can_message.data = [value]

    def send_can(self) -> None:
        """Send CAN message"""
        self.can_comm.send(self.can_message)



if __name__ == "__main__":
    app = QApplication([])
    window = AppControl()
    window.show()
    sys.exit(app.exec())
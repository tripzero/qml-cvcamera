import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtMultimedia 5.0

import RSCamera 1.0

Window {
    visible: true
    width: 1920
    height: 1080

    Rectangle {
        color: "black"
        anchors.fill: parent
    }

    RSCamera {
        id: camera
        device: 0
        fps: 30
        size: Qt.size(1920, 1080)
    }

    VideoOutput {
        anchors.fill:parent
        id: output
        source: camera
        fillMode: VideoOutput.Stretch
    }
}

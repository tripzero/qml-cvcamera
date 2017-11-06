import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtMultimedia 5.0

import CVCamera 1.0

Window {
    visible: true
    width: 1920
    height: 1080

    Rectangle {
        color: "black"
        anchors.fill: parent
    }

    CVCamera {
        id: camera
        device: 0
        size: "640x480"
    }

    VideoOutput {
        anchors.fill:parent
        id: output
        source: camera
    }
}

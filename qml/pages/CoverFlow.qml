import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Item {
    id: coverFlow

    property int topMargin: 0
    property alias model: pathView.model
    property alias currentIndex: pathView.currentIndex
    property alias currentItem: pathView.currentItem
    property alias delegate: pathView.delegate
    property alias pathItemCount: pathView.pathItemCount
    property alias path: pathView.path

    property int icount: pathView.count

    function resetIndex() {
        pathView.positionViewAtIndex(0, PathView.Center)
    }


    PathView {
        id: pathView
        anchors.fill: parent
        path: coverFlowPath
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
    }

    Path {
        id: coverFlowPath

        startX: -25
        startY: coverFlow.height / 2
        PathAttribute { name: "z"; value: 0 }
        PathAttribute { name: "angle"; value: 70 }
        PathAttribute { name: "iconScale"; value: 0.6 }

        PathLine { x: coverFlow.width * 0.35; y: coverFlow.height / 2;  }
        PathAttribute { name: "z"; value: 50 }
        PathAttribute { name: "angle"; value: 45 }
        PathAttribute { name: "iconScale"; value: 0.85 }
        PathPercent { value: 0.40 }

        PathLine { x: coverFlow.width * 0.5; y: coverFlow.height / 2;  }
        PathAttribute { name: "z"; value: 100 }
        PathAttribute { name: "angle"; value: 0 }
        PathAttribute { name: "iconScale"; value: 1.0 }

        PathLine { x: coverFlow.width * 0.65; y: coverFlow.height / 2; }
        PathAttribute { name: "z"; value: 50 }
        PathAttribute { name: "angle"; value: -45 }
        PathAttribute { name: "iconScale"; value: 0.85 }
        PathPercent { value: 0.60 }

        PathLine { x: coverFlow.width + 25; y: coverFlow.height / 2; }
        PathAttribute { name: "z"; value: 0 }
        PathAttribute { name: "angle"; value: -70 }
        PathAttribute { name: "iconScale"; value: 0.6 }
        PathPercent { value: 1.0 }
    }
}

import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false

    onStatusChanged: {
        if (status===PageStatus.Activating) {
            eqValue.value = utils.readSettings("Preset", "Rock")

            if (!loaded)
                myPlayer.loadActualEqualizer()

        }
        if (status===PageStatus.Active && !loaded) {
            loaded = true
        }
    }

    Connections {
        target: myPlayer

        onEqualizerChanged: {
            se1.value = eq.band1
            se2.value = eq.band2
            se3.value = eq.band3
            se4.value = eq.band4
            se5.value = eq.band5
            se6.value = eq.band6
            se7.value = eq.band7
            se8.value = eq.band8
            se9.value = eq.band9
            se10.value = eq.band10
        }
    }

    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.height + Theme.paddingLarge
        clip: true

        PullDownMenu {
            visible: myPlayer.eqenabled

            MenuItem {
                text: qsTr("Save preset")
                onClicked: pageStack.push("EditPreset.qml", {"editingPreset":false})
            }
        }

        Column {
            id: column
            width: parent.width
            //spacing: Theme.paddingMedium

            Item {
                width: parent.width
                height: header.height

                Header {
                    id: header
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.itemSizeMedium
                    title: qsTr("Equalizer")
                }

                Switch {
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingSmall
                    anchors.verticalCenter: header.verticalCenter
                    checked: myPlayer.eqenabled
                    onClicked: {
                        if (loaded) {
                            myPlayer.setEq(checked)
                        }
                    }
                }
            }

            ValueButton {
                id: eqValue
                enabled: myPlayer.eqenabled
                label: qsTr("Preset")
                onClicked: {
                    pageStack.push("SelectPreset.qml")
                }
            }

            Slider {
                id: se1
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(1 ,value) }
                label: "29Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se2
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(2 ,value) }
                label: "59Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se3
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(3 ,value) }
                label: "119Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se4
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(4 ,value) }
                label: "237Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se5
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(5 ,value) }
                label: "474Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se6
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(6 ,value) }
                label: "947Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se7
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(7 ,value) }
                label: "1889Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se8
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(8 ,value) }
                label: "3770Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se9
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(9 ,value) }
                label: "7523Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

            Slider {
                id: se10
                width: parent.width
                maximumValue: 12
                minimumValue: -24
                value: 0
                onValueChanged: { if (loaded) myPlayer.setEqualizer(10 ,value) }
                label: "15011Hz"
                enabled: myPlayer.eqenabled
                opacity: enabled? 1 : 0.5
            }

        }

    }

}

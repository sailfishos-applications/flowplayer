import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0

Page {
    id: page

    property var serviceList: ["org.mpris.MediaPlayer2.jolla-mediaplayer",
                               "org.mpris.MediaPlayer2.quasarmx",
                               "org.mpris.MediaPlayer2.sirensong",
                               "org.mpris.MediaPlayer2.daedalus",
                               "org.mpris.MediaPlayer2.CuteSpot",
                               "org.mpris.MediaPlayer2.CuteSpotify",
                               "org.mpris.MediaPlayer2.flowplayer"]

    Component.onCompleted: {
        mediaCombo._updating = false
        var index = serviceList.indexOf(mazeLockSettings.serviceName)
        if (index >= 0) {
            mediaCombo.currentIndex = index
        }
        else {
            mediaCombo.currentIndex = serviceList.length
        }
    }

    ConfigurationGroup {
        id: mazeLockSettings
        path: "/desktop/lipstick-jolla-home/mprisService"
        property string serviceName: "org.mpris.MediaPlayer2.jolla-mediaplayer"
        property bool useGestures: true
        property bool showProgress: true
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height
        interactive: contentHeight > height

        Column {
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: "Lockscreen player controls"
            }

            ComboBox {
                id: mediaCombo
                width: parent.width
                label: "Active media player"

                menu: ContextMenu {
                    MenuItem { text: "Jolla Mediaplayer" }
                    MenuItem { text: "QuasarMX" }
                    MenuItem { text: "SirenSong" }
                    MenuItem { text: "Daedalus" }
                    MenuItem { text: "CuteSpot" }
                    MenuItem { text: "CuteSpotify" }
                    MenuItem { text: "FlowPlayer" }
                    MenuItem { text: "Custom" }
                }

                onCurrentIndexChanged: {
                    if (currentIndex < page.serviceList.length) {
                        mazeLockSettings.serviceName = page.serviceList[currentIndex]
                    }
                }
            }

            TextField {
                width: parent.width
                text: mazeLockSettings.serviceName
                visible: mediaCombo.currentIndex == page.serviceList.length
                onVisibleChanged: if (visible) forceActiveFocus()
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onFocusChanged: {
                	if (!focus) {
                		mazeLockSettings.serviceName = text
                	}
                }
                EnterKey.enabled: true
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
    			EnterKey.onClicked: focus = false
            }

            TextSwitch {
                width: parent.width
                text: "Use gestures to switch tracks"
                checked: mazeLockSettings.useGestures
                onClicked: mazeLockSettings.useGestures = checked
            }

            TextSwitch {
                width: parent.width
                text: "Show current position progress"
                checked: mazeLockSettings.showProgress
                onClicked: mazeLockSettings.showProgress = checked
            }

        }
    }
}

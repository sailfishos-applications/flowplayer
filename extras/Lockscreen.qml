/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Vesa Halttunen <vesa.halttunen@jollamobile.com>
**
****************************************************************************/

// This file bears some issues, see issue #102 //

import QtQuick 2.0
import org.nemomobile.lipstick 0.1
import Sailfish.Silica 1.0
import com.jolla.lipstick 0.1
import org.nemomobile.time 1.0
import org.nemomobile.configuration 1.0
import MeeGo.QOfono 0.2
import "../notifications"
import "../scripts/desktop.js" as Desktop

MouseArea {
    id: lockScreen
    property alias clockHeight: clock.height
    property bool fullyOnDisplay: dashboard.contentY == 0 && Lipstick.compositor.homeVisible
    property bool onDisplay: dashboard.contentY < dashboard.lockscreenAndStatusAreaHeight && Lipstick.compositor.homeVisible
    property bool recentlyOnDisplay

    // Allows start-up wizard to not become backgrounded when the display turns off/on
    property int forceTopWindowProcessId: -1

    // Use a binding rather than a Connections element to shortcut spurious change signals.
    property bool lockscreenVisible: lipstickSettings.lockscreenVisible

    // Suffix that should be added to all theme icons that are shown in low power mode
    property string iconSuffix: lipstickSettings.lowPowerMode ? ('?' + Theme.highlightColor) : ''

    // Text color of items that are shown in low power mode
    property color textColor: lipstickSettings.lowPowerMode ? Theme.highlightColor : Theme.primaryColor

    property Item accessNotificationsHint

    property int currentX: 0

    onPositionChanged: {
        if (pressed && musicControl.mprisConfig.useGestures && musicControl.visible) {
            currentX = mouse.x - startX
            var preX = currentX / 4
            if ((preX < 0 && musicControl.mprisControl.canGoNext) || (preX > 0 && musicControl.mprisControl.canGoPrevious))
                x = preX
        }
    }

    onClicked: {
        if (leftActionIcon.accepted) {
            musicControl.mprisControl.next()
        }
        else if (rightActionIcon.accepted) {
            musicControl.mprisControl.previous()
        }
        mouse.accepted = x != 0
        x = 0
    }
    onCanceled: x = 0

    onOnDisplayChanged: {
        if (onDisplay) {
            if (desktop.animating) {
                onTimer.start()
            } else {
                recentlyOnDisplay = onDisplay
            }

            lockScreen.state = "showDateImmediatelyWithoutTimer"
        }
    }

    Timer {
        id: onTimer
        interval: 200
        onTriggered: lockScreen.recentlyOnDisplay = lockScreen.onDisplay
    }
    Timer {
        interval: 3000
        running: !lockScreen.onDisplay
        onTriggered: lockScreen.recentlyOnDisplay = lockScreen.onDisplay
    }
    Timer {
        id: offTimer
        interval: 50
        running: false
        onTriggered: {
            if (forceTopWindowProcessId < 0
                    || Lipstick.compositor.topmostWindow == null
                    || Lipstick.compositor.topmostWindow == forceTopWindowProcessId) {
                dashboard.snapToPage(0, true);
                Lipstick.compositor.setCurrentWindow(null);
                desktop.closeApplicationEnabled = false
                desktop.removeApplicationEnabled = false
            }
        }
    }

    onLockscreenVisibleChanged: {
        if (!lockscreenVisible) {
            if (dashboard.currentIndex === 0) {
                if (Desktop.instance.ambienceOnLockscreen) {
                    Desktop.instance.ambienceOnLockscreen = false
                } else {
                    // lockscreen visible, but we don't want to see it.
                    // reset view to switcher/launcher
                    dashboard.snapToPage(1);
                }
            }
        } else {
            // lockscreen enabled. make sure we're already there.
            offTimer.restart()
        }
    }

    Clock {
        id: clock
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    ConnectionStatusIndicator {
        anchors {
            bottom: clock.verticalCenter
            bottomMargin: Math.round(Theme.paddingSmall/2)
            left: parent.left
            leftMargin: Theme.smallIcons ? Theme.paddingSmall : Theme.paddingLarge
        }
    }

    Image {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        source: "image://theme/graphic-wallpaper-dimmer"
    }

    Item {
        id: networkNameAndDateLabels
        anchors {
            top: parent.top
            topMargin: Theme.paddingLarge
        }
        width: parent.width
        height: Math.max(cellularNetworkNameStatusIndicator.height, dateLabel.height)

        CellularNetworkNameStatusIndicator {
            id: cellularNetworkNameStatusIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 1 - dateLabel.opacity
        }

        Label {
            id: dateLabel
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - (Screen.width / 5 * 2)
            text: {
                var dateString = Format.formatDate(wallClock.time, Format.DateFull)
                return dateString.charAt(0).toUpperCase() + dateString.substr(1)
            }
            color: Theme.highlightColor
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            wrapMode: Text.WordWrap
            WallClock {
                id: wallClock
                enabled: lockScreen.onDisplay
                updateFrequency: WallClock.Day
            }
            Behavior on opacity {
                id: dateOpacityBehavior
                FadeAnimation { }
            }
        }
    }

    SimToolkitIdleModeIndicator {
        anchors {
            top: networkNameAndDateLabels.bottom
            topMargin: Theme.paddingMedium
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - 2*(Theme.iconSizeLarge + Theme.paddingLarge)
    }

    ProfileStatusIndicator {
        id: profileStatusIndicator
        anchors {
            bottom: clock.verticalCenter
            bottomMargin: Math.round(Theme.paddingSmall/2)
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
    }

    AlarmStatusIndicator {
        anchors {
            top: clock.verticalCenter
            topMargin: Math.round(Theme.paddingSmall/2)
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
    }

    BluetoothStatusIndicator {
        anchors {
            top: clock.verticalCenter
            topMargin: Math.round(Theme.paddingSmall/2)
            left: parent.left
            leftMargin: Theme.paddingLarge
        }
    }

    JollaNotificationListModel {
        id: horizontalNotificationListModel
        filters: [ {
            "property": "category",
            "comparator": "match",
            "value": "^x-nemo.system-update"
        } ]
    }

    JollaNotificationListModel {
        id: verticalNotificationListModel
        filters: [ {
            "property": "category",
            "comparator": "!match",
            "value": "^x-nemo.system-update"
        }, {
            "property": "priority",
            "comparator": ">=",
            "value": 100
        } ]
    }

    ConfigurationValue {
        id: weatherBannerHeight
        key: "/desktop/lipstick-jolla-home/weather_banner_height"
    }
    HorizontalNotificationList {
        y: weatherBannerHeight.value
        id: horizontalNotificationList
        model: horizontalNotificationListModel
        iconOnly: true
        onClicked: showAccessNotificationsHint()
    }
    Item {
        id: voicemailAvailable

        anchors.top: horizontalNotificationList.bottom
        width: parent.width
        height: content.height
        opacity: height / content.height

        state: ofonoMessageWaiting.voicemailWaiting ? "" : "suppressed"
        states: State {
            name: "suppressed"
            PropertyChanges {
                target: voicemailAvailable
                height: 0
            }
        }
        transitions: Transition {
            to: "*"
            SmoothedAnimation {
                target: voicemailAvailable
                properties: "height"
                duration: lockScreen.animationDuration
                easing.type: Easing.InOutQuad
            }
        }

        Column {
            id: content
            width: parent.width

            Repeater {
                model: 1
                delegate: ListItem {
                    id: notificationDelegate

                    property var notificationList: ({
                        "iconOnly": true,
                        "iconSuffix": lockScreen.iconSuffix,
                        "textColor": lockScreen.textColor,
                        "timestampUpdatesEnabled": false
                    })

                    property var modelData: ({
                        //% "Voicemail available"
                        "summary": qsTrId("lipstick-jolla-home-la-voicemail_available"),
                        "body": "",
                        "icon": "icon-lock-voicemail",
                        "itemCount": 1,
                        "timestamp": undefined
                    })

                    property QtObject object
                    property real iconSize: Theme.iconSizeLarge / 2

                    width: parent.width
                    contentHeight: verticalNotificationList.notificationHeight

                    Component.onCompleted: _showPress = false

                    onClicked: showAccessNotificationsHint()

                    NotificationItem {}
                }
            }
        }

        OfonoMessageWaiting {
            id: ofonoMessageWaiting

            property OfonoManager ofonoManager: OfonoManager {}

            modemPath: ofonoManager.modems.length ? ofonoManager.modems[0] : ""
        }
    }
    NotificationLockScreenView {
        id: verticalNotificationList
        model: verticalNotificationListModel
        notificationLimit: 4
        height: Math.min(verticalNotificationListModel.itemCount * notificationHeight,
                         notificationLimit * notificationHeight)
        anchors.top: voicemailAvailable.bottom
        iconSuffix: lockScreen.iconSuffix
        textColor: lockScreen.textColor
        onClicked: showAccessNotificationsHint()
    }
    Image {
        x: Theme.paddingLarge
        source: "image://theme/icon-lock-more" + lockScreen.iconSuffix
        anchors.top: verticalNotificationList.bottom
        /* Hide the more notifications icon when there aren't too many notifications */
        opacity: verticalNotificationListModel.itemCount <= verticalNotificationList.notificationLimit ? 0 : 1
        Behavior on opacity { FadeAnimation {} }
        visible: opacity > 0.0
    }

    function showAccessNotificationsHint() {
        if (!accessNotificationsHint) {
            var component = Qt.createComponent(Qt.resolvedUrl("AccessNotificationsHint.qml"))
            if (component.status == Component.Ready) {
                accessNotificationsHint = component.createObject(desktop)
                accessNotificationsHint.finished.connect(function() {
                    lockScreen.accessNotificationsHint = null
                })
            } else {
                console.warn("AccessNotificationsHint.qml instantiation failed " + component.errorString())
            }
        }
    }

    Item {
        id: musicControl
        anchors.bottom: ongoingCall.enabled ? ongoingCall.top : clock.top
        width: parent.width
        height: column.height
        property QtObject mprisControl
        property QtObject mprisConfig
        property QtObject feedbackEffect
        property string mprisService: "org.mpris.MediaPlayer2.jolla-mediaplayer"
        property bool displayActive: true
        property bool mprisActive: mprisControl && mprisControl.playbackStatus.length > 0

        enabled: lockscreenVisible || lipstickSettings.lowPowerMode
        visible: mprisControl ? mprisActive : false
        onEnabledChanged: {
            if (mprisControl) {
                mprisControl.active = enabled && displayActive
            }
            if (!enabled) {
                mediaName.setDefaultMarqueeOffset()
            }
        }
        opacity: (lockScreen.width - Math.abs(lockScreen.x) * 8) / lockScreen.width

        Component.onCompleted: {
            mprisConfig = Qt.createQmlObject('import QtQuick 2.0; import org.nemomobile.configuration 1.0; ConfigurationGroup {path: "/desktop/lipstick-jolla-home/mprisService"; property string serviceName: musicControl.mprisService; property bool useGestures: true; property bool showProgress: true}', musicControl)
            mprisService = mprisConfig.serviceName
            mprisConfig.serviceNameChanged.connect(function() { mprisService = mprisConfig.serviceName; })
            mprisControl = Qt.createQmlObject('import QtQuick 2.0; import org.coderus.mpris 2.0 as Mpris2; Mpris2.Control {service: musicControl.mprisService}', musicControl)
            feedbackEffect = Qt.createQmlObject("import QtQuick 2.0; import QtFeedback 5.0; ThemeEffect { effect: ThemeEffect.Press }", musicControl);
        }

        Column {
            id: column
            anchors {
                left: parent.left
                right: parent.right
                margins: Theme.paddingLarge
            }

            Image {
                id: coverArt
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(lockScreen.width/2, lockScreen.height/2)
                height: width
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
                visible: coverArt.status!==Image.Error && mprisService==="flowplayer"
                cache: false
            }

            Item {
                id: spacer
                visible: coverArt.visible
                width: parent.width
                height: Theme.paddingLarge
            }

            Item {
                id: marqueeItem
                height: mediaName.height
                width: parent.width
                clip: true

                Label {
                    id: mediaName
                    property bool shouldMarquee: false
                    property int marqueeOffset: 0
                    text: musicControl.mprisControl.metadata["xesam:title"] ? (musicControl.mprisControl.metadata["xesam:artist"] + " - " + musicControl.mprisControl.metadata["xesam:title"]) : ""
                    color: textColor
                    onTextChanged: {
                        setDefaultMarqueeOffset()
                        if (text!=="") {
                            coverArt.source = "/home/nemo/.cache/currentAlbumArt.jpeg";
                            coverArt.sourceSize.width = 0
                            coverArt.sourceSize.height = 0
                            coverArt.sourceSize.width = coverArt.width
                            coverArt.sourceSize.height = coverArt.height
                        } else {
                            coverArt.source = ""
                        }
                    }
                    x: mediaName.marqueeOffset
                    function setDefaultMarqueeOffset() {
                        marqueeOffset = shouldMarquee ? 0 : ((marqueeItem.width - width) / 2)
                        shouldMarquee = width > marqueeItem.width
                    }
                }

                Timer {
                    id: marqueeTimer
                    interval: 1000
                    running: lockscreenVisible && mediaName.shouldMarquee
                    repeat: true
                    property int offset: -1
                    onTriggered: {
                        if (mediaName.width + mediaName.marqueeOffset > marqueeItem.width) {
                            mediaName.marqueeOffset += marqueeTimer.offset
                            if (mediaName.width + mediaName.marqueeOffset <= marqueeItem.width) {
                                interval = 1000
                            }
                            else {
                                interval = 10
                            }
                        }
                        else {
                            mediaName.marqueeOffset = 0
                            interval = 1000
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: Theme.itemSizeSmall
                clip: true
                visible: musicControl.mprisConfig.showProgress

                ProgressBar {
                    id: positionProgress
                    width: parent.width
                    anchors {
                        top: parent.top
                        topMargin: - Theme.paddingLarge
                    }
                    minimumValue: 0
                    property int duration: musicControl.mprisControl.metadata["xesam:mediaDuration"] ? musicControl.mprisControl.metadata["xesam:mediaDuration"] : 1000000
                    property int length: musicControl.mprisControl.metadata["mpris:length"] ? musicControl.mprisControl.metadata["mpris:length"] : 1000000
                    maximumValue: Math.max(duration, length)
                    value: musicControl.mprisControl.position
                    valueText: Format.formatDuration(value / 1000000, Format.DurationShort)
                    property color color: Theme.highlightColor
                    layer.effect: ShaderEffect {
                        id: shaderItem
                        property color color: positionProgress.color

                        fragmentShader: "
                            varying mediump vec2 qt_TexCoord0;
                            uniform highp float qt_Opacity;
                            uniform lowp sampler2D source;
                            uniform highp vec4 color;
                            void main() {
                                highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                                gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                            }
                        "
                    }
                    layer.enabled: lipstickSettings.lowPowerMode
                    layer.samplerName: "source"
                }

                Timer {
                    interval: 1000
                    running: musicControl.mprisControl.playbackStatus == "Playing" && musicControl.mprisControl.active
                    onTriggered: musicControl.mprisControl.initialize()
                }
            }

            Row {
                height: Theme.itemSizeMedium
                visible: musicControl.displayActive
                enabled: !stupidTimer.running
                Item {
                    width: column.width / 3
                    height: parent.height

                    IconButton {
                        icon.source: "image://theme/icon-m-previous"
                        anchors.centerIn: parent
                        enabled: musicControl.mprisControl.canGoPrevious
                        onClicked: {
                            stupidTimer.start()
                            musicControl.mprisControl.previous()
                            musicControl.feedbackEffect.play()
                        }
                    }
                }
                Item {
                    width: column.width / 3
                    height: parent.height

                    IconButton {
                        icon.source: musicControl.mprisControl.playbackStatus == "Playing" ? "image://theme/icon-m-pause" : "image://theme/icon-m-play"
                        anchors.centerIn: parent
                        enabled: musicControl.mprisControl.playbackStatus == "Playing" ? musicControl.mprisControl.canPause : musicControl.mprisControl.canPlay
                        onClicked: {
                            musicControl.mprisControl.playPause()
                            musicControl.feedbackEffect.play()
                        }
                    }
                }
                Item {
                    width: column.width / 3
                    height: parent.height

                    IconButton {
                        icon.source: "image://theme/icon-m-next"
                        anchors.centerIn: parent
                        enabled: musicControl.mprisControl.canGoNext
                        onClicked: {
                            stupidTimer.start()
                            musicControl.mprisControl.next()
                            musicControl.feedbackEffect.play()
                        }
                    }
                }
                Timer {
                    id: stupidTimer
                    interval: 1000
                    onTriggered: musicControl.mprisControl.initialize()
                }
            }
        }

        Image {
            id: leftActionIcon
            property bool accepted: visible && x == (lockScreen.width / 2 - width / 2 - lockScreen.x)
            source: "image://theme/icon-cover-next-song" + (accepted ? ("?" + Theme.highlightColor) : "")
            visible: musicControl.mprisControl.canGoNext && musicControl.visible && lockScreen.x < 0
            opacity: accepted ? 1.0 : (lockScreen.width - (x + lockScreen.x)) / (lockScreen.width / 2)
            anchors.verticalCenter: musicControl.verticalCenter
            property int calculatedX: lockScreen.width + lockScreen.currentX * 3 - lockScreen.x
            x: calculatedX < (lockScreen.width / 2 - width / 2 - lockScreen.x) ? (lockScreen.width / 2 - width / 2 - lockScreen.x) : calculatedX
            onAcceptedChanged: if (accepted) musicControl.feedbackEffect.play()
        }

        Image {
            id: rightActionIcon
            rotation: 180
            property bool accepted: visible && x == (lockScreen.width / 2 - width / 2  - lockScreen.x)
            source: "image://theme/icon-cover-next-song" + (accepted ? ("?" + Theme.highlightColor) : "")
            visible: musicControl.mprisControl.canGoPrevious && musicControl.visible && lockScreen.x > 0
            opacity: accepted ? 1.0 : (x + lockScreen.x) / (lockScreen.width / 2)
            anchors.verticalCenter: musicControl.verticalCenter
            property int calculatedX: lockScreen.currentX * 3 - width - lockScreen.x
            x: calculatedX > (lockScreen.width / 2 - width / 2 - lockScreen.x) ? (lockScreen.width / 2 - width / 2 - lockScreen.x) : calculatedX
            onAcceptedChanged: if (accepted) musicControl.feedbackEffect.play()
        }
    }

    OngoingCall {
        id: ongoingCall
        anchors.bottom: clock.top
    }

    SneakPeekHint {
        id: sneakPeekHint
    }

    Connections {
        target: Lipstick.compositor
        onDisplayOff: {
            if (lockscreenVisible && onDisplay && dashboard.contentY > 0) {
                dashboard.cancelFlick()
                dashboard.snapToPage(0, true)
            }
            musicControl.displayActive = false
        }
        onDisplayOn: musicControl.displayActive = true
        onDisplayAboutToBeOn: {
            lockScreen.state = "showDateImmediately"
            sneakPeekHint.sneakPeekActive = lipstickSettings.lowPowerMode
        }
        onDisplayAboutToBeOff: sneakPeekHint.sneakPeekActive = false
    }

    Connections {
        target: lipstickSettings
        onLowPowerModeChanged: if (!lipstickSettings.lowPowerMode) sneakPeekHint.sneakPeekActive = false
    }

    onFullyOnDisplayChanged: {
        if (fullyOnDisplay && !hideDateTimer.running) {
            lockScreen.state = "showDate"
        }
    }

    Timer {
        id: hideDateTimer
        interval: 3000
        onTriggered: {
            var indicator = cellularNetworkNameStatusIndicator
            if (!lipstickSettings.lowPowerMode && (indicator.homeNetwork || indicator.visitorNetwork)) {
                lockScreen.state = "showCellularNetworkName"
            }
        }
    }

    states: [
        State {
            name: "showDateImmediatelyWithoutTimer"
            StateChangeScript {
                script: {
                    dateOpacityBehavior.enabled = false
                    dateLabel.opacity = 1
                    dateOpacityBehavior.enabled = true
                }
            }
        },
        State {
            name: "showDateImmediately"
            StateChangeScript {
                script: {
                    dateOpacityBehavior.enabled = false
                    dateLabel.opacity = 1
                    dateOpacityBehavior.enabled = true
                    hideDateTimer.start()
                }
            }
        },
        State {
            name: "showDate"
            StateChangeScript {
                script: {
                    dateLabel.opacity = 1
                    hideDateTimer.start()
                }
            }
        },
        State {
            name: "showCellularNetworkName"
            PropertyChanges {
                target: dateLabel
                opacity: 0
            }
        }
    ]
}

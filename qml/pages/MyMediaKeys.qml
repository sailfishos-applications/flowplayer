import QtQuick 2.0
import Sailfish.Media 1.0
import org.nemomobile.policy 1.0
import com.jolla.mediaplayer 1.0

Item {
    id: mediaKeys

    property bool _grabKeys: myPlayer.state > 0 && keysResource.acquired

    Permissions {
        enabled: myPlayer.state > 0
        applicationClass: "player"

        Resource {
            id: keysResource
            type: Resource.HeadsetButtons
            optional: true
        }
    }

    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaTogglePlayPause; onReleased: myPlayer.state===2? myPlayer.resume() : myPlayer.pause() }
    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaPlay; onReleased: myPlayer.play() }
    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaPause; onReleased: myPlayer.pause() }
    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaStop; onReleased: myPlayer.stop() }
    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaNext; onReleased: nowPlayingPage.nextSong()  }
    MediaKey { enabled: _grabKeys; key: Qt.Key_MediaPrevious; onReleased: nowPlayingPage.prevSong() }
    MediaKey { enabled: _grabKeys; key: Qt.Key_ToggleCallHangup; onReleased: myPlayer.state===2? myPlayer.resume() : myPlayer.pause() }

    /*MediaKey {
        id: forwardKey

        enabled: _grabKeys
        key: Qt.Key_AudioForward
        onPressed: myPlayer.forwarding = true
        onRepeat: {
            myPlayer.setSeekRepeat(true)
            myPlayer.forwarding = true
        }
        onReleased: myPlayer.forwarding = false
    }*/

    /*MediaKey {
        id: rewindKey

        enabled: _grabKeys
        key: Qt.Key_AudioRewind
        onPressed: myPlayer.rewinding = true
        onRepeat: {
            myPlayer.setSeekRepeat(true)
            myPlayer.rewinding = true
        }
        onReleased: myPlayer.rewinding = false
    }*/
}

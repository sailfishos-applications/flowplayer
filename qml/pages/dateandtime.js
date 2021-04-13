function getDate(milliseconds) {
    /* Convert the date to a string */

    var date = new Date();
    date.setTime(milliseconds);
    var dateString = date.getFullYear() + "-" + (date.getMonth() + 1) + "-" + date.getDate();
    return dateString;
}

function getDuration(secs) {
    /* Converts seconds to HH:MM:SS format. */
    var hours = Math.floor(secs / 3600);
    var minutes = Math.floor(secs / 60) - (hours * 60);
    var seconds = secs - (hours * 3600) - ( minutes * 60);
    if (seconds < 10) {
        seconds = "0" + seconds;
    }
    var duration = minutes + ":" + seconds;
    if (hours > 0) {
        duration = hours + ":" + duration;
    }
    return duration;
}

function getTime(msecs) {
    /* Convert milliseconds to HH:MM:SS format */

    var secs = Math.floor(msecs / 1000);
    var hours = Math.floor(secs / 3600);
    var minutes = Math.floor(secs / 60) - (hours * 60);
    var seconds = secs - (hours * 3600) - (minutes * 60);
    if (seconds < 10) {
        seconds = "0" + seconds;
    }
    var time = minutes + ":" + seconds;
    if (hours > 0) {
        time = hours + ":" + time;
    }
    return time;
}

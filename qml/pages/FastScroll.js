// FastScroll.js - this is just SectionScroller.js with a fix for
// section.criteria == ViewSection.FirstCharacter
var sectionData = [];
var _sections = [];

function initialize(list) {
    initSectionData(list);
}

function initSectionData(list) {
    if (!list || !list.model) return;
    sectionData = [];
    _sections = [];
    var current = "",
        prop = list.section.property,
        item;



    if (list.section.criteria == ViewSection.FullString) {
        for (var i = 0, count = list.count; i < count; i++) {
            list.currentIndex = i;
            item = list.currentItem.myData.band
            if (item !== current) {
                current = item;
                _sections.push(current);
                sectionData.push({ index: i, header: current });
            }
        }
    } else if (list.section.criteria == ViewSection.FirstCharacter) {
        for (var i2 = 0, count2 = list.count; i2 < count2; i2++) {
            list.currentIndex = i2;
            item = list.currentItem.myData.title.substring(0,1)
            if (item !== current) {
                current = item;
                _sections.push(current);
                sectionData.push({ index: i2, header: current });
            }
        }
    }
}

function getSectionPositionString(name) {
    var val = _sections.indexOf(name);
    return val === 0 ? "first" :
           val === _sections.length - 1 ? "last" : false;
}

function getAt(pos) {
    return _sections[pos] ? _sections[pos] : "";
}

function getRelativeSections(current) {
    var val = _sections.indexOf(current),
        sect = [],
        sl = _sections.length;

    val = val < 1 ? 1 : val >= sl-1 ? sl-2 : val;
    sect = [getAt(val - 1), getAt(val), getAt(val + 1)];

    return sect;
}

function getClosestSection(pos, down) {
    var tmp = (_sections.length) * pos;
    var val = Math.ceil(tmp) // TODO: better algorithm
    val = val < 2 ? 1 : val;
    return _sections[val-1];
}

function getNextSection(current) {
    var val = _sections.indexOf(current);
    return (val > -1 ? _sections[(val < _sections.length - 1 ? val + 1 : val)] : _sections[0]) || "";
}

function getPreviousSection(current) {
    var val = _sections.indexOf(current);
    return (val > -1 ? _sections[(val > 0 ? val - 1 : val)] : _sections[0]) || "";
}

function getIndexFor(sectionName) {
    var val = sectionData[_sections.indexOf(sectionName)].index;
    return val === 0 || val > 0 ? val : -1;
}

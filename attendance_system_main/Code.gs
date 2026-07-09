/*
  Google Apps Script for ESP32 Fingerprint Attendance

  Sheet tabs required:
  1. Users
     Column A: Fingerprint ID
     Column B: Name

  2. Attendance
     Column A: Date
     Column B: Time
     Column C: Fingerprint ID
     Column D: Name

  Deploy as a Web App:
  - Execute as: Me
  - Who has access: Anyone
*/

function doPost(e) {
  try {
    if (!e || !e.postData || !e.postData.contents) {
      return jsonResponse({ status: 'error', message: 'No POST data' });
    }

    var data = JSON.parse(e.postData.contents);
    var fingerprintId = Number(data.fingerprint_id);

    if (!fingerprintId) {
      return jsonResponse({ status: 'error', message: 'Invalid fingerprint_id' });
    }

    var spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
    var usersSheet = spreadsheet.getSheetByName('Users');
    var attendanceSheet = spreadsheet.getSheetByName('Attendance');

    if (!usersSheet || !attendanceSheet) {
      return jsonResponse({ status: 'error', message: 'Missing sheet tab' });
    }

    var name = findUserName(usersSheet, fingerprintId);

    if (!name) {
      return jsonResponse({ status: 'error', message: 'User not found' });
    }

    var now = new Date();
    var timeZone = Session.getScriptTimeZone();
    var dateText = Utilities.formatDate(now, timeZone, 'yyyy-MM-dd');
    var timeText = Utilities.formatDate(now, timeZone, 'HH:mm:ss');

    attendanceSheet.appendRow([dateText, timeText, fingerprintId, name]);

    return jsonResponse({ status: 'success', message: 'Attendance saved' });
  } catch (error) {
    return jsonResponse({ status: 'error', message: error.message });
  }
}

function findUserName(usersSheet, fingerprintId) {
  var lastRow = usersSheet.getLastRow();

  if (lastRow < 2) {
    return '';
  }

  var values = usersSheet.getRange(2, 1, lastRow - 1, 2).getValues();

  for (var i = 0; i < values.length; i++) {
    var sheetFingerprintId = Number(values[i][0]);
    var name = values[i][1];

    if (sheetFingerprintId === fingerprintId) {
      return name;
    }
  }

  return '';
}

function jsonResponse(data) {
  return ContentService
    .createTextOutput(JSON.stringify(data))
    .setMimeType(ContentService.MimeType.JSON);
}

function doGet() {
  return jsonResponse({ status: 'success', message: 'Attendance web app is running' });
}

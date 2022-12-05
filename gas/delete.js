function mainDelete() {
  let date = new Date()
  date.setDate(date.getDate() - 7)
  var formatDate = Utilities.formatDate(date, "JST","yyMMdd"); // 削除対象日
  ss = myAllSheetName();
  var actictivess = SpreadsheetApp.getActive();
  for(let i = 0; i < ss.length; i++){
    console.log(ss[i])
    if(formatDate > ss[i].slice(0,6)){
      exportCsv(ss[i]);
      deleteSheet(ss[i]);
    }　else if(isNaN(ss[i].slice(0,6))){
      const sheet = actictivess.getSheetByName(ss[i]);
      var range = sheet.getRange('A1');
      if(range.isBlank()){
        console.info(`削除実行`)
        deleteSheet(ss[i]);//数字以外が含まれるいる場合は削除
      }
    }
  }
}

function myAllSheetName() {
  const sheets = SpreadsheetApp.getActiveSpreadsheet().getSheets();
  return sheets.map(sheet => sheet.getName());
}

function exportCsv(sheetName) {

  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheetByName(sheetName);

  var values = sheet.getDataRange().getValues();
  var csv = values.join('\n');

  var blob = Utilities.newBlob(csv, MimeType.CSV, sheetName + '.csv');
  var id = '1188jJm6ngcuekSZdgLsuSju1Vrrppcp_'; //フォルダID
  var folder = DriveApp.getFolderById(id);
  folder.createFile(blob);
}

function deleteSheet(sheetName){

// 現在アクティブなスプレッドシートを取得
  var ss = SpreadsheetApp.getActive();
  var sheet = ss.getSheetByName(sheetName);
// シートを削除
  ss.deleteSheet(sheet);
}

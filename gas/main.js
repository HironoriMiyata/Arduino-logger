function main(id, data) {
  var formatDate = Utilities.formatDate(new Date(), "JST","yyMMdd");
  const name = formatDate + id;
  var nowDate = Utilities.formatDate(new Date(), "JST","yyyy-MM-dd kk:mm:ss");
  newSheet(name);
  const value = [
    [
      nowDate,
      data
    ]
  ];
  addNewLine(name, value);
}

function newSheet(name) {
  const spId = '10OIw7BLnE4tEWGEkX90dJWsauefINVMht1Y8vZUFYFM';
  let insert_ss = SpreadsheetApp.openById(spId);

  const ss = SpreadsheetApp.getActiveSpreadsheet().getSheetByName(name);
  if(ss === null){
    const newSheet = insert_ss.insertSheet();
    newSheet.setName(name)
  }
}

function addNewLine(name, value){
    var ss = SpreadsheetApp.getActiveSpreadsheet();
    var sheet = ss.getSheetByName(name);
    var lastRow = sheet.getLastRow();
    const numRows = value.length;
    const numColumns = value[0].length;
    sheet.getRange(lastRow + 1, 1, numRows, numColumns).setValues(value);
}

function doGet(e){
  const id = e.parameter.id;
  const data = e.parameter.value;

  if(id === null || data === null){
    console.log("data is null");
    return 0;
  }
  main(id, data);
}

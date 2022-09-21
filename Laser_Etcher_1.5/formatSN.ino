void formatSn() {
  if ((serLeng == 10) && (serNum <= 4294967296)) {       //9 digit serNum
    if (serNum >= 4294967296) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0000000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000000000" + String(serNum);
    }
  }
  if ((serLeng == 9) && (serNum <= 1000000000)) {       //9 digit serNum
    if (serNum == 1000000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00000000" + String(serNum);
    }
  }
  if ((serLeng == 8) && (serNum <= 100000000)) {       //8 digit serNum
    if (serNum == 100000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0000000" + String(serNum);
    }
  }
  if ((serLeng == 7) && (serNum <= 10000000)) {       //7 digit serNum
    if (serNum == 10000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000000" + String(serNum);
    }
  }
  if ((serLeng == 6) && (serNum <= 1000000)) {        //6 digit serNum
    if (serNum == 1000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00000" + String(serNum);
    }
  }
  if ((serLeng == 5) && (serNum <= 100000)) {        //5 digit serNum
    if (serNum == 100000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0000" + String(serNum);
    }
  }
  if ((serLeng == 4) && (serNum <= 10000)) {        //4 digit serNum
    if (serNum == 10000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000" + String(serNum);
    }
  }
  if ((serLeng == 3) && (serNum <= 1000)) {        //3 digit serNum
    if (serNum == 1000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00" + String(serNum);
    }
  }
  if ((serLeng == 2) && (serNum <= 100)) {            //2 digit serNum
    if (serNum == 100) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0" + String(serNum);
    }
  }
  if ((serLeng == 1) && (serNum <= 10)) {           //1 digit serNum
    if (serNum == 10) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
  }
}

void drawGreg_TS(uint16_t y)   // Draw Time
{
  char BuffH[3];
  char BuffM[3];
  static long   lsRn;
  long          Tmr = millis();
  dmd.selectFont(System5x7);
  sprintf(BuffH, "%02d", hour);
  sprintf(BuffM, "%02d", minute);
  dmd.drawString(1, y, BuffH, 2, GRAPHICS_NORMAL);
  dmd.drawString(20, y, BuffM, 2, GRAPHICS_NORMAL);

  if (Tmr - lsRn < 500) //dmd.drawLine((x-10)+1,7,(x-10)+9,7,1);
  {
    //    dmd.drawFilledBox( 15, 0, 16, 6, 1 );
    dmd.drawChar(  14,  y, ':', GRAPHICS_OR );

  } else {
    dmd.drawChar(  14,  y, ':', GRAPHICS_NOR );
  }
  if (Tmr - lsRn > 1000) lsRn = Tmr;

}

void blinkBlock(int DrawAdd)
{
  // check RunSelector
  if (!dwDo(DrawAdd)) return;

  static uint32_t   lsRn;
  static uint16_t   ct, ct_l;
  int               mnt, scd;
  uint32_t          Tmr = millis();
  char              locBuff[6];

  if (jumat)       {
    ct_l = Prm.JM * 60;
  }
  else            {
    ct_l = Prm.SO * 60;
  }


  if ((Tmr - lsRn) > 1000)
  { lsRn = Tmr;
    ct++;
  }
  if ((ct % 2) == 0)
  {
    dmd.writePixel( 15,  8, GRAPHICS_OR, 1);
  } else {
    dmd.clearScreen( true );
  }

  if (ct > ct_l)
  { dwDone(DrawAdd);
    azzan = false;
    jumat = false;
    ct = 0;
    dmd.clearScreen( true );
  }
}

// =========================================
// Drawing Tools============================
// =========================================

boolean dwDo(int DrawAdd)
{ if (RunSel == DrawAdd) {
    return true;
  }
  else return false;
}

void dwDone(int DrawAdd)
{ RunFinish = DrawAdd;
  RunSel = 0;
}


void Buzzer(uint8_t state)
{
  if (state == 1 and Prm.BZ == 1)
  {
    digitalWrite(BUZZ, HIGH);
  }
  else
  {
    digitalWrite(BUZZ, LOW);
  }
}

void bz()
{
  static uint32_t   lsRn;
  static uint8_t    b;
  uint32_t          Tmr = millis();
  if (bzzz == 1) {
    lsRn = Tmr;
    bzzz = 0;
    b = 1;
  }
  if ((Tmr - lsRn) < 200)
  {
    //    lsRn = Tmr;
    Buzzer(1);
  } else {
    if (b == 1) {
      b = 0;
      Buzzer(0);
    }
  }
}

void fType(int x)
{
  if (x == 0) dmd.selectFont(Font0);
  else if (x == 1) dmd.selectFont(Font1);
  else if (x == 2) dmd.selectFont(Font2);
  else if (x == 3) dmd.selectFont(Font3);
  else if (x == 4) dmd.selectFont(Font4);
  else dmd.selectFont(Font5);
}

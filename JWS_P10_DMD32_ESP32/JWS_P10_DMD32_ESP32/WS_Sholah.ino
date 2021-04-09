void drawAzzan(int DrawAdd)
{
  // check RunSelector
  if (!dwDo(DrawAdd)) return;
  uint8_t           ct_limit = 18; //harus angka genap
  static uint8_t    ct;
  static uint32_t   lsRn;
  uint32_t          Tmr = millis();

  if ((Tmr - lsRn) > 500 and ct <= ct_limit)
  {
    lsRn = Tmr;
    DoSwap = true;
    ct++;
  }

  if ((ct % 2) == 0)
  {
    fType(1);
    dmd.drawString(1, 0, "ADZAN", 5, GRAPHICS_NORMAL);
    if (jumat) {
      fType(1);
      dmd.drawString(1, 9, sholatN(8), 5, GRAPHICS_NORMAL);
    }
    else      {
      fType(1);
      dmd.drawString(1, 9, sholatN(SholatNow), 5, GRAPHICS_NORMAL);
    }
    Buzzer(1);

  }
  else
  {
    Buzzer(0);
    dmd.clearScreen( true );
  }
  if ((Tmr - lsRn) > 2000 and (ct > ct_limit))
  { dwDone(DrawAdd);
    ct = 0;
    Buzzer(0);
    dmd.clearScreen( true );
  }

}


void drawIqomah(int DrawAdd)  // Countdown Iqomah (9 menit)
{
  // check RunSelector
  if (!dwDo(DrawAdd)) return;

  static uint32_t   lsRn;
  uint32_t          Tmr = millis();
  static int        ct;
  int               mnt, scd, cn_l ;
  char              locBuff[6];

  cn_l  = (Iqomah[SholatNow] * 60);

  if ((Tmr - lsRn) > 1000 and ct <= cn_l)
  {
    lsRn = Tmr;


    if (ct > (cn_l - 10)) Buzzer(1); // Buzzer on 2 seccon before Iqomah
    ct++;

    DoSwap = true;
  }
  mnt = floor((cn_l - ct) / 60);
  scd = (cn_l - ct) % 60;
  if (mnt > 0) {
    fType(1);
    sprintf(locBuff, "%02d:%02d", mnt, scd);
    dmd.drawString(1, 9, locBuff, 5, GRAPHICS_NORMAL);
  }
  else    {
    //    matrix.setCursor(27, 11 + 16);
    fType(1);
    sprintf(locBuff, "%02d:%02d", mnt, scd);
    dmd.drawString(1, 9, locBuff, 5, GRAPHICS_NORMAL);
  }
  fType(1);
  if ((ct % 2) == 0) {
    dmd.drawString(1, 0, "IQOMAH", 5, GRAPHICS_OR);
  } else {
    dmd.drawString(1, 0, "IQOMAH", 5, GRAPHICS_NOR);
    Buzzer(0);
  }
  if (ct > cn_l)
  {
    dwDone(DrawAdd);
    ct = 0;
    Buzzer(0);
    dmd.clearScreen( true );
  }
}

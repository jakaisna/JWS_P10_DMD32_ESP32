void dwMrq(const char* msg, int Speed, int dDT, int DrawAdd) //running teks ada jam nya
{
  // check RunSelector
  static uint16_t   x;

  if (!dwDo(DrawAdd)) return;

  if (reset_x != 0) {
    x = 0;
    reset_x = 0;
  }

  if (x == 0) {
    if (dDT == 1) {
      fType(1);
      dmd.drawMarquee(msg, strlen(msg), (32 * dmdLAYS_ACROSS) + 2, 0);
    }
    if (dDT == 2) {
      fType(1);
      dmd.drawMarquee(msg, strlen(msg), (32 * dmdLAYS_ACROSS) + 2, 9);
    }
    if (dDT == 3) {
      fType(0);
      dmd.drawMarquee(msg, strlen(msg), (32 * dmdLAYS_ACROSS) + 2, 0);
    }
  }

  static long   lsRn;
  long          Tmr = millis();
  if (ret == false) {
    if ((Tmr - lsRn) > Speed)
    { lsRn = Tmr;

      x++;
      if (dDT == 1)
      {
        ret = dmd.stepSplitMarquee3(0, 7, 0);
        drawGreg_TS(9);
      }
      else if (dDT == 2) //jam yang diatas
      {
        ret = dmd.stepSplitMarquee3(8, 15, 0);
        drawGreg_TS(0);
      }
      else
      {
        fType(0);
        ret = dmd.stepSplitMarquee3(0, 15, 0);

      }

    }
  } else {
    ret = false;
    x = 0;
    dmd.clearScreen( true );
    dwDone(DrawAdd);
  }
}

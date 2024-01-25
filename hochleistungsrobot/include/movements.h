// Spider_mini (Top View)
//  -----     Front     -----
// |  L4 |             |  L1 |
// | GP6 |             | GP0 |
//  ----- -----   ----- -----
//       |     | |     |
//       | GP7 | | GP1 |
//        -----   -----
//       |     | |     |
//       | GP8 | | GP2 |
//  ----- -----   ----- -----
// |  L4 |             |  L2 |
// | GP9 |             | GP3 |
//  -----               -----
#ifndef MOVEMENTS_H
#define MOVEMENTS_H

class Movements {

public:
  const static int N_ACTIONS = 9; // Number of action code elements

  const static int ZERO_N_STEPS = 1;
  int zero[ZERO_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {25, 90, 90, 155, 155, 90, 90, 25, 1000}, // zero position
  };

  // Check up
  const static int CHECK_UP_N_STEPS = 2;
  int checkUp[CHECK_UP_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {15, 90, 90, 165, 165, 90, 90, 15, 200}, // standby
      {0, 0, 0, -70, 0, 0, 0, 70, 400},        // leg2,4 up
  };

  const static int FORWARD_N_STEPS = 11;
  int forward[FORWARD_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {25, 90, 90, 155, 155, 90, 90, 25, 100}, // zero position
      {20, 0, 0, 0, 0, 0, -45, 20, 100},       // leg1,4 up; leg4 fw
      {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg1,4 dn
      {0, 0, 0, -20, -20, 0, 0, 0, 100},       // leg2,3 up
      {0, -45, 45, 0, 0, 0, 45, 0, 100},       // leg1,4 bk; leg2 fw
      {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg2,3 dn
      {20, 45, 0, 0, 0, 0, 0, 20, 100},        // leg1,4 up; leg1 fw
      {0, 0, -45, 0, 0, 45, 0, 0, 100},        // leg2,3 bk
      {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg1,4 dn
      {0, 0, 0, 0, -20, 0, 0, 0, 100},         // leg3 up
      {0, 0, 0, 0, 20, -45, 0, 0, 100},        // leg3 fw dn
  };

  // Move Right
  const static int CRAB_RIGHT_N_STEPS = 11;
  int crabRight[CRAB_RIGHT_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {30, 90, 90, 150, 150, 90, 90, 30, 100}, // standby
      {0, 0, 0, -20, -20, -45, 0, 0, 100},     // leg2,3 up; leg3 fw
      {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg2,3 dn
      {20, 0, 0, 0, 0, 0, 0, 20, 100},         // leg4,1 up
      {0, 0, -70, 0, 0, 45, 65, 0, 100},       // leg2,3 bk; leg4 fw
      {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg4,1 dn
      {0, 0, 70, -20, -20, 0, 0, 0, 100},      // leg2,3 up; leg2 fw
      {0, 45, 0, 0, 0, 0, -65, 0, 100},        // leg4,1 bk
      {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg2,3 dn
      {20, 0, 0, 0, 0, 0, 0, 0, 100},          // leg1 up
      {-20, -45, 0, 0, 0, 0, 0, 0, 100},       // leg1 fw dn
  };

  // Backward
  const static int BACK_N_STEPS = 11;
  int back[BACK_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {30, 90, 90, 150, 150, 90, 90, 30, 100}, // standby
      {20, -45, 0, 0, 0, 0, 0, 20, 100},       // leg4,1 up; leg1 fw
      {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg4,1 dn
      {0, 0, 0, -20, -20, 0, 0, 0, 100},       // leg3,2 up
      {0, 45, 0, 0, 0, 65, -65, 0, 100},       // leg4,1 bk; leg3 fw
      {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg3,2 dn
      {20, 0, 0, 0, 0, 0, 65, 20, 100},        // leg4,1 up; leg4 fw
      {0, 0, 45, 0, 0, -65, 0, 0, 100},        // leg3,1 bk
      {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg4,1 dn
      {0, 0, 0, -20, 0, 0, 0, 0, 100},         // leg2 up
      {0, 0, -45, 20, 0, 0, 0, 0, 100},        // leg2 fw dn
  };

  // Climb
  const static int CLIMB_N_STEPS = 33;
  int climb[CLIMB_N_STEPS][N_ACTIONS] = {
      // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms
      {15, 90, 90, 165, 165, 90, 90, 15, 200}, // standby
      {20, 0, 0, 0, 0, 0, -50, 20, 200},       // leg1,4 up; leg4 fw
      {-20, 0, 0, 0, 0, 0, 0, -20, 200},       // leg1,4 dn
      {0, 0, 0, -20, -50, 0, 0, 0, 200},       // leg2,3 up (gp4- 50)
      {0, -50, 50, 0, 0, 0, 50, 0, 200},       // leg1,4 bk; leg2 fw
      {0, 0, 0, 20, 50, 0, 0, 0, 200},         // leg2,3 dn
      {50, 50, 0, 0, 0, 0, 0, 20, 200},        // leg1,4 up; leg1 fw
      {0, 0, -50, 0, 0, 50, 0, 0, 200},        // leg2,3 bk
      {-50, 0, 0, 0, 0, 0, 0, -20, 200},       // leg1,4 dn
      {0, 0, 0, 0, -20, 0, 0, 0, 200},         // leg3 up
      {0, 0, 0, 0, 20, -50, 0, 0, 200},        // leg3 fw dn
      {20, 0, 0, 0, -20, 0, 0, 0, 200},        // leg1,3 up   baixa frente
      {40, 0, 0, 0, 0, 0, -50, 20, 200},       // leg1,4 up; leg4 fw
      {-40, 0, 0, 0, 0, 0, 0, -20, 200},       // leg1,4 dn
      {0, 0, 0, -20, -40, 0, 0, 0, 200},       // leg2,3 up
      {0, -50, 50, 0, 0, 0, 70, 0, 200},       // leg1,4 bk; leg2 fw
      {0, 0, 0, 20, 40, 0, 0, 0, 200},         // leg2,3 dn
      {20, 50, 0, 0, 0, 0, 0, 20, 200},        // leg1,4 up; leg1 fw
      {0, 0, -70, 0, 0, 50, 0, 0, 200},        // leg2,3 bk
      {-20, 0, 0, 0, 0, 0, 0, -20, 200},       // leg1,4 dn
      {0, 0, 0, 0, 0, 0, 0, 50, 200},          // leg4 up começa patas de trás
      {0, 0, 0, 0, 0, 0, -65, 0, 200},         // leg4 fw
      {0, 0, 0, 0, 0, 0, 0, -50, 200},         // leg4 dn
      {0, 0, 0, 0, 0, -70, 0, 0, 200},         // leg3 fw
      {0, 0, 0, 0, 0, 70, 65, 0, 200},         // leg3,4 bk
      {10, 0, 0, 0, -10, 0, 0, 0, 200},        // leg1,3 up   baixa frente
      {0, 0, 0, -50, 0, 0, 0, 0, 200},         // leg2 up
      {0, 0, 65, 0, 0, 0, 0, 0, 200},          // leg2 fw
      {0, 0, 0, 50, 0, 0, 0, 0, 200},          // leg2 dn
      {0, -70, -65, 0, 0, 0, 0, 0, 200},       // leg1,2 bk
      {60, 0, 0, 0, -60, 0, 0, 0, 200},        // leg1,3 up   //baixa tudo
      {0, 70, 0, 0, 0, -70, 0, 0, 200},        // leg1,3 fw  //reset
      {-90, 0, 0, 0, 90, 0, 0, 0, 200},        // leg1,3 dn  //sobe frente
  };
};

#endif // MOVEMENTS_H

/**/
/*   // Move Left */
/*   const int MoveleftStep = 11; */
/*   const int Moveleft[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 90, 90, 150, 150, 90, 90, 30, 100}, // standby */
/*       {0, 0, -45, -20, -20, 0, 0, 0, 100},     // leg3,2 up; leg2 fw */
/*       {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg3,2 dn */
/*       {20, 0, 0, 0, 0, 0, 0, 20, 100},         // leg1,4 up */
/*       {0, 65, 45, 0, 0, -65, 0, 0, 100},       // leg3,2 bk; leg1 fw */
/*       {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg1,4 dn */
/*       {0, 0, 0, -20, -20, 65, 0, 0, 100},      // leg3,2 up; leg3 fw */
/*       {0, -65, 0, 0, 0, 0, 45, 0, 100},        // leg1,4 bk */
/*       {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg3,2 dn */
/*       {0, 0, 0, 0, 0, 0, 0, 20, 100},          // leg4 up */
/*       {0, 0, 0, 0, 0, 0, -45, -20, 100},       // leg4 fw dn */
/*   // Standby */
/*   const int standbySet = 2; */
/*   int standby[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {90, 90, 90, 90, 90, 90, 90, 90, 200}, // prep standby */
/*       {-20, 0, 0, 20, 20, 0, 0, -20, 200},   // standby */
/*   }; */
/**/
/**/
/*   // Forward */
/**/
/**/
/**/
/*   // Turn left */
/*   const int TurnleftStep = 8; */
/*   const int Turnleft[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 90, 90, 150, 150, 90, 90, 30, 100}, // standby */
/*       {20, 0, 0, 0, 0, 0, 0, 20, 100},         // leg1,4 up */
/*       {0, 45, 0, 0, 0, 0, 45, 0, 100},         // leg1,4 turn */
/*       {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg1,4 dn */
/*       {0, 0, 0, -20, -20, 0, 0, 0, 100},       // leg2,3 up */
/*       {0, 0, 45, 0, 0, 45, 0, 0, 100},         // leg2,3 turn */
/*       {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg2,3 dn */
/*       {0, -45, -45, 0, 0, -45, -45, 0, 100},   // leg1,2,3,4 turn */
/*   }; */
/**/
/*   // Turn right */
/*   const int TurnrightStep = 8; */
/*   const int Turnright[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 90, 90, 150, 150, 90, 90, 30, 100}, // standby */
/*       {0, 0, 0, -20, -20, 0, 0, 0, 100},       // leg2,3 up */
/*       {0, 0, -45, 0, 0, -45, 0, 0, 100},       // leg2,3 turn */
/*       {0, 0, 0, 20, 20, 0, 0, 0, 100},         // leg2,3 dn */
/*       {20, 0, 0, 0, 0, 0, 0, 20, 100},         // leg1,4 up */
/*       {0, -45, 0, 0, 0, 0, -45, 0, 100},       // leg1,4 turn */
/*       {-20, 0, 0, 0, 0, 0, 0, -20, 100},       // leg1,4 dn */
/*       {0, 45, 45, 0, 0, 45, 45, 0, 100},       // leg1,2,3,4 turn */
/*   }; */
/**/
/*   // Lie */
/*   const int LieStep = 6; */
/*   const int Lie[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 90, 90, 150, 150, 80, 90, 30, 100}, // standby */
/*       {150, 0, 0, -20, 0, 0, 0, 20, 200},      // leg1 maxup and leg2,4
   up */
/*       {0, 45, 0, 0, 0, 0, 0, 0, 350},          // leg1 fw */
/*       {0, -45, 0, 0, 0, 0, 0, 0, 350},         // leg1 bk */
/*       {0, 45, 0, 0, 0, 0, 0, 0, 350},          // leg1 fw */
/*       {0, -45, 0, 0, 0, 0, 0, 0, 350}          // leg1 bk */
/*   }; */
/**/
/*   // Say Hi */
/*   const int SayhiStep = 4; */
/*   const int Sayhi[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {0, 90, 90, 150, 180, 90, 90, 30, 200}, // leg1, 3 down */
/*       {30, 0, 0, 0, -30, 0, 0, 0, 200},       // standby */
/*       {-30, 0, 0, 0, 30, 0, 0, 0, 200},       // leg1, 3 down */
/*       {30, 0, 0, 0, -30, 0, 0, 0, 200},       // standby */
/*   }; */
/**/
/*   // Fighting */
/*   const int FightingStep = 11; */
/*   const int Fighting[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {0, 90, 90, 180, 150, 90, 90, 30, 200},      // leg1, 2 down */
/*       {0, -20, -20, 0, 0, -20, -20, 0, 200},       // body turn left */
/*       {0, 40, 40, 0, 0, 40, 40, 0, 200},           // body turn right */
/*       {0, -40, -40, 0, 0, -40, -40, 0, 200},       // body turn left */
/*       {0, 40, 40, 0, 0, 40, 40, 0, 200},           // body turn right */
/*       {30, -20, -20, -20, 30, -20, -20, -20, 200}, // leg1, 2 up ; leg3,
   4 down */
/*       {0, -20, -20, 0, 0, -20, -20, 0, 200},       // body turn left */
/*       {0, 40, 40, 0, 0, 40, 40, 0, 200},           // body turn right */
/*       {0, -40, -40, 0, 0, -40, -40, 0, 200},       // body turn left */
/*       {0, 40, 40, 0, 0, 40, 40, 0, 200},           // body turn right */
/*       {0, -20, -20, 0, 0, -20, -20, 0, 200},       // leg1, 2 up ; leg3,
   4 down */
/*   }; */
/**/
/*   // Push up */
/*   const int PushupStep = 11; */
/*   const int Pushup[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 45, 38, 150, 150, 135, 147, 30, 300}, // start position */
/*       {30, 0, 0, -40, -30, 0, 0, 0, 400},        // down */
/*       {-30, 0, 0, 40, 30, 0, 0, 0, 500},         // up */
/*       {30, 0, 0, 0, -30, 0, 0, 40, 600},         // down */
/*       {-30, 0, 0, 0, 30, 0, 0, -40, 700},        // up */
/*       {30, 0, 0, -40, -30, 0, 0, 0, 1300},       // down */
/*       {-30, 0, 0, 40, 30, 0, 0, 0, 1800},        // up */
/*       {45, 0, 0, -30, -45, 0, 0, 30, 200},       // fast down */
/*       {-45, 0, 0, 0, 10, 0, 0, 0, 500},          // leg1 up */
/*       {0, 0, 0, 0, 35, 0, 0, 0, 500},            // leg2 up */
/*       {0, 0, 0, 30, 0, 0, 0, -30, 500},          // leg3, leg4 up */
/*   }; */
/**/
/*   // Sleep */
/*   const int SleepStep = 2; */
/*   const int Sleep[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {0, 90, 90, 150, 150, 90, 90, 0, 400}, // leg1,4 dn */
/*       {0, -45, 45, 0, 0, 45, -45, 0, 400},   // protect myself */
/*   }; */
/**/
/*   // Dancing 1 */
/*   const int Dance1Step = 10; */
/*   const int Dance1[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 90, 90, 150, 150, 90, 90, 30, 300}, // leg1,2,3,4 up */
/*       {-30, 0, 0, 0, 0, 0, 0, 0, 300},         // leg1 dn */
/*       {30, 0, 0, 30, 0, 0, 0, 0, 300},         // leg1 up; leg2 dn */
/*       {0, 0, 0, -30, 0, 0, 0, -30, 300},       // leg2 up; leg4 dn */
/*       {0, 0, 0, 0, 30, 0, 0, 30, 300},         // leg4 up; leg3 dn */
/*       {-30, 0, 0, 0, -30, 0, 0, 0, 300},       // leg3 up; leg1 dn */
/*       {30, 0, 0, 30, 0, 0, 0, 0, 300},         // leg1 up; leg2 dn */
/*       {0, 0, 0, -30, 0, 0, 0, -30, 300},       // leg2 up; leg4 dn */
/*       {0, 0, 0, 0, 30, 0, 0, 30, 300},         // leg4 up; leg3 dn */
/*       {0, 0, 0, 0, -30, 0, 0, 0, 300},         // leg3 up */
/*   }; */
/**/
/*   // Dancing 2 */
/*   const int Dance2Step = 9; */
/*   const int Dance2[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 45, 135, 150, 150, 135, 45, 30, 300}, // leg1,2,3,4 two sides
 */
/*       {30, 0, 0, -30, 0, 0, 0, 0, 300},          // leg1,2 up */
/*       {-30, 0, 0, 30, -30, 0, 0, 30, 300},       // leg1,2 dn; leg3,4 up
 */
/*       {30, 0, 0, -30, 30, 0, 0, -30, 300},       // leg3,4 dn; leg1,2 up
 */
/*       {-30, 0, 0, 30, -30, 0, 0, 30, 300},       // leg1,2 dn; leg3,4 up
 */
/*       {30, 0, 0, -30, 30, 0, 0, -30, 300},       // leg3,4 dn; leg1,2 up
 */
/*       {-30, 0, 0, 30, -30, 0, 0, 30, 300},       // leg1,2 dn; leg3,4 up
 */
/*       {30, 0, 0, -30, 30, 0, 0, -30, 300},       // leg3,4 dn; leg1,2 up
 */
/*       {-25, 0, 0, 25, 0, 0, 0, 0, 300},          // leg1,2 dn */
/*   }; */
/**/
/*   // Dancing 3 */
/*   const int Dance3Step = 10; */
/*   const int Dance3[][N_ACTIONS] = { */
/*       // GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7,  ms */
/*       {30, 45, 38, 150, 150, 135, 147, 30, 300}, // leg1,2,3,4 bk */
/*       {30, 0, 0, -40, -30, 0, 0, 0, 300},        // leg1,2,3 up */
/*       {-30, 0, 0, 40, 30, 0, 0, 0, 300},         // leg1,2,3 dn */
/*       {30, 0, 0, 0, -30, 0, 0, 40, 300},         // leg1,3,4 up */
/*       {-30, 0, 0, 0, 30, 0, 0, -40, 300},        // leg1,3,4 dn */
/*       {30, 0, 0, -40, -30, 0, 0, 0, 300},        // leg1,2,3 up */
/*       {-30, 0, 0, 40, 30, 0, 0, 0, 300},         // leg1,2,3 dn */
/*       {30, 0, 0, 0, -30, 0, 0, 40, 300},         // leg1,3,4 up */
/*       {-30, 0, 0, 0, 30, 0, 0, -40, 300},        // leg1,3,4 dn */
/*       {0, 45, 45, 0, 0, -45, -45, 0, 300},       // standby */
/*   }; */

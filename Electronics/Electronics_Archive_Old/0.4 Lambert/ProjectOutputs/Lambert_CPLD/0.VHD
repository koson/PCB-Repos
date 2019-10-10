------------------------------------------------------------
-- VHDL X_0_4_Lambert_PLD_Schematic
-- 2009 9 17 18 4 10
-- Created By "Altium Designer VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL X_0_4_Lambert_PLD_Schematic
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

Entity X_0_4_Lambert_PLD_Project Is
  port
  (
    CCW_LIMIT : In    STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=CCW_LIMIT
    CLK       : In    STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=CLK
    CW_LIMIT  : In    STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=CW_LIMIT
    DIRECTION : In    STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=DIRECTION
    IN1       : Out   STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=IN1
    IN2       : Out   STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=IN2
    PWM_OUT   : Out   STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=PWM_OUT
    START     : In    STD_LOGIC;                             -- ObjectKind=Port|PrimaryId=START
    SW        : In    STD_LOGIC_VECTOR(7 DOWNTO 0)           -- ObjectKind=Port|PrimaryId=SW[7..0]
  );
  attribute MacroCell : boolean;

End X_0_4_Lambert_PLD_Project;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of X_0_4_Lambert_PLD_Project is
   Component AND2S                                           -- ObjectKind=Part|PrimaryId=U12|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U12-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U12-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U12-O
      );
   End Component;

   Component CB8CLEDB                                        -- ObjectKind=Part|PrimaryId=U9|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-C
        CE  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-CE
        CEO : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-CEO
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-CLR
        D   : in  STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U9-D[7..0]
        L   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-L
        Q   : out STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U9-Q[7..0]
        TC  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U9-TC
        UP  : in  STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U9-UP
      );
   End Component;

   Component CDIV8DC50                                       -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U3-CLKIN
      );
   End Component;

   Component CDIV10DC50                                      -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U4-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U4-CLKIN
      );
   End Component;

   Component CDIV256                                         -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U2-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U2-CLKIN
      );
   End Component;

   Component FD2CS                                           -- ObjectKind=Part|PrimaryId=U11|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U11-C
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U11-CLR
        D0  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U11-D0
        D1  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U11-D1
        Q0  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U11-Q0
        Q1  : out STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U11-Q1
      );
   End Component;

   Component FDC                                             -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U1-C
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U1-CLR
        D   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U1-D
        Q   : out STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U1-Q
      );
   End Component;

   Component INV                                             -- ObjectKind=Part|PrimaryId=U8|SecondaryId=1
      port
      (
        I : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U8-I
        O : out STD_LOGIC                                    -- ObjectKind=Pin|PrimaryId=U8-O
      );
   End Component;

   Component OR2S                                            -- ObjectKind=Part|PrimaryId=U14|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U14-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U14-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U14-O
      );
   End Component;


    Signal NamedSignal_2_5MHZ   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=2.5MHZ
    Signal PinSignal_U1_Q       : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_Q
    Signal PinSignal_U10_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU10_O
    Signal PinSignal_U11_Q0     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_Q0
    Signal PinSignal_U11_Q1     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_Q1
    Signal PinSignal_U12_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU12_O
    Signal PinSignal_U13_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU12_I0
    Signal PinSignal_U14_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_CLR
    Signal PinSignal_u15_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_I0
    Signal PinSignal_U16_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=Netu15_I0
    Signal PinSignal_U2_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_C
    Signal PinSignal_U3_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=2.5MHZ
    Signal PinSignal_U4_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU4_CLKDV
    Signal PinSignal_U5_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU5_CLKDV
    Signal PinSignal_U6_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU6_CLKDV
    Signal PinSignal_U7_CLKDV   : STD_LOGIC; -- ObjectKind=Net|PrimaryId=250_HZ
    Signal PinSignal_U8_O       : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU8_O
    Signal PinSignal_U9_TC      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_CLR
    Signal PowerSignal_GND      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=GND
    Signal PowerSignal_VCC      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=VCC

begin
    U16 : INV                                                -- ObjectKind=Part|PrimaryId=U16|SecondaryId=1
      Port Map
      (
        I => CCW_LIMIT,                                      -- ObjectKind=Pin|PrimaryId=U16-I
        O => PinSignal_U16_O                                 -- ObjectKind=Pin|PrimaryId=U16-O
      );

    u15 : AND2S                                              -- ObjectKind=Part|PrimaryId=u15|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U16_O,                               -- ObjectKind=Pin|PrimaryId=u15-I0
        I1 => PinSignal_U11_Q1,                              -- ObjectKind=Pin|PrimaryId=u15-I1
        O  => PinSignal_u15_O                                -- ObjectKind=Pin|PrimaryId=u15-O
      );

    U14 : OR2S                                               -- ObjectKind=Part|PrimaryId=U14|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_u15_O,                               -- ObjectKind=Pin|PrimaryId=U14-I0
        I1 => PinSignal_U12_O,                               -- ObjectKind=Pin|PrimaryId=U14-I1
        O  => PinSignal_U14_O                                -- ObjectKind=Pin|PrimaryId=U14-O
      );

    U13 : INV                                                -- ObjectKind=Part|PrimaryId=U13|SecondaryId=1
      Port Map
      (
        I => CW_LIMIT,                                       -- ObjectKind=Pin|PrimaryId=U13-I
        O => PinSignal_U13_O                                 -- ObjectKind=Pin|PrimaryId=U13-O
      );

    U12 : AND2S                                              -- ObjectKind=Part|PrimaryId=U12|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U13_O,                               -- ObjectKind=Pin|PrimaryId=U12-I0
        I1 => PinSignal_U11_Q0,                              -- ObjectKind=Pin|PrimaryId=U12-I1
        O  => PinSignal_U12_O                                -- ObjectKind=Pin|PrimaryId=U12-O
      );

    U11 : FD2CS                                              -- ObjectKind=Part|PrimaryId=U11|SecondaryId=1
      Port Map
      (
        C   => START,                                        -- ObjectKind=Pin|PrimaryId=U11-C
        CLR => PinSignal_U14_O,                              -- ObjectKind=Pin|PrimaryId=U11-CLR
        D0  => PinSignal_U10_O,                              -- ObjectKind=Pin|PrimaryId=U11-D0
        D1  => DIRECTION,                                    -- ObjectKind=Pin|PrimaryId=U11-D1
        Q0  => PinSignal_U11_Q0,                             -- ObjectKind=Pin|PrimaryId=U11-Q0
        Q1  => PinSignal_U11_Q1                              -- ObjectKind=Pin|PrimaryId=U11-Q1
      );

    U10 : INV                                                -- ObjectKind=Part|PrimaryId=U10|SecondaryId=1
      Port Map
      (
        I => DIRECTION,                                      -- ObjectKind=Pin|PrimaryId=U10-I
        O => PinSignal_U10_O                                 -- ObjectKind=Pin|PrimaryId=U10-O
      );

    U9 : CB8CLEDB                                            -- ObjectKind=Part|PrimaryId=U9|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U8_O,                               -- ObjectKind=Pin|PrimaryId=U9-C
        CE  => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U9-CE
        CLR => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U9-CLR
        D   => SW,                                           -- ObjectKind=Pin|PrimaryId=U9-D[7..0]
        L   => PinSignal_U2_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U9-L
        TC  => PinSignal_U9_TC,                              -- ObjectKind=Pin|PrimaryId=U9-TC
        UP  => PowerSignal_GND                               -- ObjectKind=Pin|PrimaryId=U9-UP
      );

    U8 : INV                                                 -- ObjectKind=Part|PrimaryId=U8|SecondaryId=1
      Port Map
      (
        I => NamedSignal_2_5MHZ,                             -- ObjectKind=Pin|PrimaryId=U8-I
        O => PinSignal_U8_O                                  -- ObjectKind=Pin|PrimaryId=U8-O
      );

    U7 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U7|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U7_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U7-CLKDV
        CLKIN => PinSignal_U6_CLKDV                          -- ObjectKind=Pin|PrimaryId=U7-CLKIN
      );

    U6 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U6|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U6_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U6-CLKDV
        CLKIN => PinSignal_U5_CLKDV                          -- ObjectKind=Pin|PrimaryId=U6-CLKIN
      );

    U5 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U5_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U5-CLKDV
        CLKIN => PinSignal_U4_CLKDV                          -- ObjectKind=Pin|PrimaryId=U5-CLKIN
      );

    U4 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U4_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U4-CLKDV
        CLKIN => PinSignal_U3_CLKDV                          -- ObjectKind=Pin|PrimaryId=U4-CLKIN
      );

    U3 : CDIV8DC50                                           -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U3_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN => CLK                                         -- ObjectKind=Pin|PrimaryId=U3-CLKIN
      );

    U2 : CDIV256                                             -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U2_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U2-CLKDV
        CLKIN => NamedSignal_2_5MHZ                          -- ObjectKind=Pin|PrimaryId=U2-CLKIN
      );

    U1 : FDC                                                 -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U2_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U1-C
        CLR => PinSignal_U9_TC,                              -- ObjectKind=Pin|PrimaryId=U1-CLR
        D   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U1-D
        Q   => PinSignal_U1_Q                                -- ObjectKind=Pin|PrimaryId=U1-Q
      );

    -- Signal Assignments
    ---------------------
    IN1                <= PinSignal_U11_Q0; -- ObjectKind=Net|PrimaryId=NetU11_Q0
    IN2                <= PinSignal_U11_Q1; -- ObjectKind=Net|PrimaryId=NetU11_Q1
    NamedSignal_2_5MHZ <= PinSignal_U3_CLKDV; -- ObjectKind=Net|PrimaryId=2.5MHZ
    PowerSignal_GND    <= '0'; -- ObjectKind=Net|PrimaryId=GND
    PowerSignal_VCC    <= '1'; -- ObjectKind=Net|PrimaryId=VCC
    PWM_OUT            <= PinSignal_U1_Q; -- ObjectKind=Net|PrimaryId=NetU1_Q

end structure;
------------------------------------------------------------


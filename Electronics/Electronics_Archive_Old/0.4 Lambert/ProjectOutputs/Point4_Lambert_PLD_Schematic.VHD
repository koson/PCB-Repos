------------------------------------------------------------
-- VHDL Point4_Lambert_PLD_Schematic
-- 2014 1 21 10 36 37
-- Created By "DXP VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL Point4_Lambert_PLD_Schematic
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

--synthesis translate_off
Library GENERIC_LIB;
Use     GENERIC_LIB.all;

--synthesis translate_on
Entity Point4_Lambert_PLD_Schematic Is
  port
  (
    CLK_20MHZ     : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=CLK_20MHZ
    DC_DIRECTION  : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=DC_DIRECTION
    DEC_AXIS_OK   : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=DEC_AXIS_OK
    DEPLOYED_1    : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=DEPLOYED_1
    IN1           : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=IN1
    IN2           : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=IN2
    IR_STRIP      : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=IR_STRIP
    LIGHTBARMINUS : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=LIGHTBAR-
    OK_TO_MOVE    : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=OK_TO_MOVE
    PWM_D         : In    STD_LOGIC_VECTOR(7 DOWNTO 0);      -- ObjectKind=Port|PrimaryId=PWM_D[7..0]
    PWM_IR_LOAD   : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_IR_LOAD
    PWM_IR_OUT    : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_IR_OUT
    PWM_UV_LOAD   : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_UV_LOAD
    PWM_UV_OUT    : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_UV_OUT
    PWM_WHT_LOAD  : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_WHT_LOAD
    PWM_WHT_OUT   : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=PWM_WHT_OUT
    STOWED_1      : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=STOWED_1
    TCK           : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=TCK
    TDI           : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=TDI
    TDO_PLD       : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=TDO_PLD
    TMS           : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=TMS
    UV_STRIP      : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=UV_STRIP
    X_DC_START    : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=~DC_START
    X_HOME        : Out   STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=~HOME
    X_PWM_IR_EN   : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=~PWM_IR_EN
    X_PWM_UV_EN   : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=~PWM_UV_EN
    X_PWM_WHT_EN  : In    STD_LOGIC;                         -- ObjectKind=Port|PrimaryId=~PWM_WHT_EN
    X_RA_AXIS_OK  : In    STD_LOGIC                          -- ObjectKind=Port|PrimaryId=~RA_AXIS_OK
  );
  attribute MacroCell : boolean;

End Point4_Lambert_PLD_Schematic;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of Point4_Lambert_PLD_Schematic is
   Component AND2S                                           -- ObjectKind=Part|PrimaryId=U15|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U15-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U15-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U15-O
      );
   End Component;

   Component CB8CLEDB                                        -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-C
        CE  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-CE
        CEO : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-CEO
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-CLR
        D   : in  STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U5-D[7..0]
        L   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-L
        Q   : out STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U5-Q[7..0]
        TC  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U5-TC
        UP  : in  STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U5-UP
      );
   End Component;

   Component CDIV128DC50                                     -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U2-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U2-CLKIN
      );
   End Component;

   Component CDIV256                                         -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U3-CLKIN
      );
   End Component;

   Component FD2CS                                           -- ObjectKind=Part|PrimaryId=U14|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U14-C
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U14-CLR
        D0  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U14-D0
        D1  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U14-D1
        Q0  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U14-Q0
        Q1  : out STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U14-Q1
      );
   End Component;

   Component FD8B                                            -- ObjectKind=Part|PrimaryId=U6|SecondaryId=1
      port
      (
        C : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U6-C
        D : in  STD_LOGIC_VECTOR(7 downto 0);                -- ObjectKind=Pin|PrimaryId=U6-D[7..0]
        Q : out STD_LOGIC_VECTOR(7 downto 0)                 -- ObjectKind=Pin|PrimaryId=U6-Q[7..0]
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

   Component INV                                             -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      port
      (
        I : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U4-I
        O : out STD_LOGIC                                    -- ObjectKind=Pin|PrimaryId=U4-O
      );
   End Component;

   Component LDC                                             -- ObjectKind=Part|PrimaryId=U23|SecondaryId=1
      port
      (
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U23-CLR
        D   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U23-D
        G   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U23-G
        Q   : out STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U23-Q
      );
   End Component;

   Component OR2S                                            -- ObjectKind=Part|PrimaryId=U19|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U19-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U19-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U19-O
      );
   End Component;

   Component XOR2S                                           -- ObjectKind=Part|PrimaryId=U30|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U30-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U30-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U30-O
      );
   End Component;


    Signal PinSignal_U1_Q           : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_Q
    Signal PinSignal_U10_Q          : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=NetU10_Q[7..0]
    Signal PinSignal_U11_Q          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_Q
    Signal PinSignal_U12_TC         : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_CLR
    Signal PinSignal_U13_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU13_O
    Signal PinSignal_U14_Q0         : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_Q0
    Signal PinSignal_U14_Q1         : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_Q1
    Signal PinSignal_U15_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_C
    Signal PinSignal_U16_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU16_O
    Signal PinSignal_U17_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU15_I1
    Signal PinSignal_U18_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU18_O
    Signal PinSignal_U19_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_CLR
    Signal PinSignal_U2_CLKDV       : STD_LOGIC; -- ObjectKind=Net|PrimaryId=156.25 KHZ
    Signal PinSignal_u20_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU19_I0
    Signal PinSignal_U21_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU21_O
    Signal PinSignal_U22_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU22_O
    Signal PinSignal_U23_Q          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU23_Q
    Signal PinSignal_U24_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU23_CLR
    Signal PinSignal_U25_Q          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU25_Q
    Signal PinSignal_U26_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU25_CLR
    Signal PinSignal_U27_Q          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU27_Q
    Signal PinSignal_U28_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU27_CLR
    Signal PinSignal_U29_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU5_CE
    Signal PinSignal_U3_CLKDV       : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU3_CLKDV
    Signal PinSignal_U30_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU29_I1
    Signal PinSignal_U31_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU9_CE
    Signal PinSignal_U32_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU29_I0
    Signal PinSignal_U33_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU30_I1
    Signal PinSignal_U34_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU12_CE
    Signal PinSignal_U35_O          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU15_I0
    Signal PinSignal_U4_O           : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_C
    Signal PinSignal_U5_TC          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_CLR
    Signal PinSignal_U6_Q           : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=NetU5_D[7..0]
    Signal PinSignal_U7_Q           : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=NetU7_Q[7..0]
    Signal PinSignal_U8_Q           : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU8_Q
    Signal PinSignal_U9_TC          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU8_CLR
    Signal PowerSignal_GND          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=GND
    Signal PowerSignal_VCC          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=VCC

begin
    U35 : INV                                                -- ObjectKind=Part|PrimaryId=U35|SecondaryId=1
      Port Map
      (
        I => X_DC_START,                                     -- ObjectKind=Pin|PrimaryId=U35-I
        O => PinSignal_U35_O                                 -- ObjectKind=Pin|PrimaryId=U35-O
      );

    U34 : INV                                                -- ObjectKind=Part|PrimaryId=U34|SecondaryId=1
      Port Map
      (
        I => X_PWM_WHT_EN,                                   -- ObjectKind=Pin|PrimaryId=U34-I
        O => PinSignal_U34_O                                 -- ObjectKind=Pin|PrimaryId=U34-O
      );

    U33 : INV                                                -- ObjectKind=Part|PrimaryId=U33|SecondaryId=1
      Port Map
      (
        I => X_PWM_UV_EN,                                    -- ObjectKind=Pin|PrimaryId=U33-I
        O => PinSignal_U33_O                                 -- ObjectKind=Pin|PrimaryId=U33-O
      );

    U32 : INV                                                -- ObjectKind=Part|PrimaryId=U32|SecondaryId=1
      Port Map
      (
        I => X_PWM_IR_EN,                                    -- ObjectKind=Pin|PrimaryId=U32-I
        O => PinSignal_U32_O                                 -- ObjectKind=Pin|PrimaryId=U32-O
      );

    U31 : AND2S                                              -- ObjectKind=Part|PrimaryId=U31|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U30_O,                               -- ObjectKind=Pin|PrimaryId=U31-I0
        I1 => PinSignal_U33_O,                               -- ObjectKind=Pin|PrimaryId=U31-I1
        O  => PinSignal_U31_O                                -- ObjectKind=Pin|PrimaryId=U31-O
      );

    U30 : XOR2S                                              -- ObjectKind=Part|PrimaryId=U30|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U32_O,                               -- ObjectKind=Pin|PrimaryId=U30-I0
        I1 => PinSignal_U33_O,                               -- ObjectKind=Pin|PrimaryId=U30-I1
        O  => PinSignal_U30_O                                -- ObjectKind=Pin|PrimaryId=U30-O
      );

    U29 : AND2S                                              -- ObjectKind=Part|PrimaryId=U29|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U32_O,                               -- ObjectKind=Pin|PrimaryId=U29-I0
        I1 => PinSignal_U30_O,                               -- ObjectKind=Pin|PrimaryId=U29-I1
        O  => PinSignal_U29_O                                -- ObjectKind=Pin|PrimaryId=U29-O
      );

    U28 : INV                                                -- ObjectKind=Part|PrimaryId=U28|SecondaryId=1
      Port Map
      (
        I => PinSignal_U34_O,                                -- ObjectKind=Pin|PrimaryId=U28-I
        O => PinSignal_U28_O                                 -- ObjectKind=Pin|PrimaryId=U28-O
      );

    U27 : LDC                                                -- ObjectKind=Part|PrimaryId=U27|SecondaryId=1
      Port Map
      (
        CLR => PinSignal_U28_O,                              -- ObjectKind=Pin|PrimaryId=U27-CLR
        D   => PinSignal_U11_Q,                              -- ObjectKind=Pin|PrimaryId=U27-D
        G   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U27-G
        Q   => PinSignal_U27_Q                               -- ObjectKind=Pin|PrimaryId=U27-Q
      );

    U26 : INV                                                -- ObjectKind=Part|PrimaryId=U26|SecondaryId=1
      Port Map
      (
        I => PinSignal_U31_O,                                -- ObjectKind=Pin|PrimaryId=U26-I
        O => PinSignal_U26_O                                 -- ObjectKind=Pin|PrimaryId=U26-O
      );

    U25 : LDC                                                -- ObjectKind=Part|PrimaryId=U25|SecondaryId=1
      Port Map
      (
        CLR => PinSignal_U26_O,                              -- ObjectKind=Pin|PrimaryId=U25-CLR
        D   => PinSignal_U8_Q,                               -- ObjectKind=Pin|PrimaryId=U25-D
        G   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U25-G
        Q   => PinSignal_U25_Q                               -- ObjectKind=Pin|PrimaryId=U25-Q
      );

    U24 : INV                                                -- ObjectKind=Part|PrimaryId=U24|SecondaryId=1
      Port Map
      (
        I => PinSignal_U29_O,                                -- ObjectKind=Pin|PrimaryId=U24-I
        O => PinSignal_U24_O                                 -- ObjectKind=Pin|PrimaryId=U24-O
      );

    U23 : LDC                                                -- ObjectKind=Part|PrimaryId=U23|SecondaryId=1
      Port Map
      (
        CLR => PinSignal_U24_O,                              -- ObjectKind=Pin|PrimaryId=U23-CLR
        D   => PinSignal_U1_Q,                               -- ObjectKind=Pin|PrimaryId=U23-D
        G   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U23-G
        Q   => PinSignal_U23_Q                               -- ObjectKind=Pin|PrimaryId=U23-Q
      );

    U22 : INV                                                -- ObjectKind=Part|PrimaryId=U22|SecondaryId=1
      Port Map
      (
        I => UV_STRIP,                                       -- ObjectKind=Pin|PrimaryId=U22-I
        O => PinSignal_U22_O                                 -- ObjectKind=Pin|PrimaryId=U22-O
      );

    U21 : INV                                                -- ObjectKind=Part|PrimaryId=U21|SecondaryId=1
      Port Map
      (
        I => LIGHTBARMINUS,                                  -- ObjectKind=Pin|PrimaryId=U21-I
        O => PinSignal_U21_O                                 -- ObjectKind=Pin|PrimaryId=U21-O
      );

    u20 : AND2S                                              -- ObjectKind=Part|PrimaryId=u20|SecondaryId=1
      Port Map
      (
        I0 => DEPLOYED_1,                                    -- ObjectKind=Pin|PrimaryId=u20-I0
        I1 => PinSignal_U14_Q1,                              -- ObjectKind=Pin|PrimaryId=u20-I1
        O  => PinSignal_u20_O                                -- ObjectKind=Pin|PrimaryId=u20-O
      );

    U19 : OR2S                                               -- ObjectKind=Part|PrimaryId=U19|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_u20_O,                               -- ObjectKind=Pin|PrimaryId=U19-I0
        I1 => PinSignal_U18_O,                               -- ObjectKind=Pin|PrimaryId=U19-I1
        O  => PinSignal_U19_O                                -- ObjectKind=Pin|PrimaryId=U19-O
      );

    U18 : AND2S                                              -- ObjectKind=Part|PrimaryId=U18|SecondaryId=1
      Port Map
      (
        I0 => STOWED_1,                                      -- ObjectKind=Pin|PrimaryId=U18-I0
        I1 => PinSignal_U14_Q0,                              -- ObjectKind=Pin|PrimaryId=U18-I1
        O  => PinSignal_U18_O                                -- ObjectKind=Pin|PrimaryId=U18-O
      );

    U17 : AND2S                                              -- ObjectKind=Part|PrimaryId=U17|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U16_O,                               -- ObjectKind=Pin|PrimaryId=U17-I0
        I1 => DEC_AXIS_OK,                                   -- ObjectKind=Pin|PrimaryId=U17-I1
        O  => PinSignal_U17_O                                -- ObjectKind=Pin|PrimaryId=U17-O
      );

    U16 : INV                                                -- ObjectKind=Part|PrimaryId=U16|SecondaryId=1
      Port Map
      (
        I => X_RA_AXIS_OK,                                   -- ObjectKind=Pin|PrimaryId=U16-I
        O => PinSignal_U16_O                                 -- ObjectKind=Pin|PrimaryId=U16-O
      );

    U15 : AND2S                                              -- ObjectKind=Part|PrimaryId=U15|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U35_O,                               -- ObjectKind=Pin|PrimaryId=U15-I0
        I1 => PinSignal_U17_O,                               -- ObjectKind=Pin|PrimaryId=U15-I1
        O  => PinSignal_U15_O                                -- ObjectKind=Pin|PrimaryId=U15-O
      );

    U14 : FD2CS                                              -- ObjectKind=Part|PrimaryId=U14|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U15_O,                              -- ObjectKind=Pin|PrimaryId=U14-C
        CLR => PinSignal_U19_O,                              -- ObjectKind=Pin|PrimaryId=U14-CLR
        D0  => PinSignal_U13_O,                              -- ObjectKind=Pin|PrimaryId=U14-D0
        D1  => DC_DIRECTION,                                 -- ObjectKind=Pin|PrimaryId=U14-D1
        Q0  => PinSignal_U14_Q0,                             -- ObjectKind=Pin|PrimaryId=U14-Q0
        Q1  => PinSignal_U14_Q1                              -- ObjectKind=Pin|PrimaryId=U14-Q1
      );

    U13 : INV                                                -- ObjectKind=Part|PrimaryId=U13|SecondaryId=1
      Port Map
      (
        I => DC_DIRECTION,                                   -- ObjectKind=Pin|PrimaryId=U13-I
        O => PinSignal_U13_O                                 -- ObjectKind=Pin|PrimaryId=U13-O
      );

    U12 : CB8CLEDB                                           -- ObjectKind=Part|PrimaryId=U12|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U2_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U12-C
        CE  => PinSignal_U34_O,                              -- ObjectKind=Pin|PrimaryId=U12-CE
        CLR => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U12-CLR
        D   => PinSignal_U10_Q,                              -- ObjectKind=Pin|PrimaryId=U12-D[7..0]
        L   => PinSignal_U3_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U12-L
        TC  => PinSignal_U12_TC,                             -- ObjectKind=Pin|PrimaryId=U12-TC
        UP  => PowerSignal_GND                               -- ObjectKind=Pin|PrimaryId=U12-UP
      );

    U11 : FDC                                                -- ObjectKind=Part|PrimaryId=U11|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U4_O,                               -- ObjectKind=Pin|PrimaryId=U11-C
        CLR => PinSignal_U12_TC,                             -- ObjectKind=Pin|PrimaryId=U11-CLR
        D   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U11-D
        Q   => PinSignal_U11_Q                               -- ObjectKind=Pin|PrimaryId=U11-Q
      );

    U10 : FD8B                                               -- ObjectKind=Part|PrimaryId=U10|SecondaryId=1
      Port Map
      (
        C => PWM_WHT_LOAD,                                   -- ObjectKind=Pin|PrimaryId=U10-C
        D => PWM_D,                                          -- ObjectKind=Pin|PrimaryId=U10-D[7..0]
        Q => PinSignal_U10_Q                                 -- ObjectKind=Pin|PrimaryId=U10-Q[7..0]
      );

    U9 : CB8CLEDB                                            -- ObjectKind=Part|PrimaryId=U9|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U2_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U9-C
        CE  => PinSignal_U31_O,                              -- ObjectKind=Pin|PrimaryId=U9-CE
        CLR => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U9-CLR
        D   => PinSignal_U7_Q,                               -- ObjectKind=Pin|PrimaryId=U9-D[7..0]
        L   => PinSignal_U3_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U9-L
        TC  => PinSignal_U9_TC,                              -- ObjectKind=Pin|PrimaryId=U9-TC
        UP  => PowerSignal_GND                               -- ObjectKind=Pin|PrimaryId=U9-UP
      );

    U8 : FDC                                                 -- ObjectKind=Part|PrimaryId=U8|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U4_O,                               -- ObjectKind=Pin|PrimaryId=U8-C
        CLR => PinSignal_U9_TC,                              -- ObjectKind=Pin|PrimaryId=U8-CLR
        D   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U8-D
        Q   => PinSignal_U8_Q                                -- ObjectKind=Pin|PrimaryId=U8-Q
      );

    U7 : FD8B                                                -- ObjectKind=Part|PrimaryId=U7|SecondaryId=1
      Port Map
      (
        C => PWM_UV_LOAD,                                    -- ObjectKind=Pin|PrimaryId=U7-C
        D => PWM_D,                                          -- ObjectKind=Pin|PrimaryId=U7-D[7..0]
        Q => PinSignal_U7_Q                                  -- ObjectKind=Pin|PrimaryId=U7-Q[7..0]
      );

    U6 : FD8B                                                -- ObjectKind=Part|PrimaryId=U6|SecondaryId=1
      Port Map
      (
        C => PWM_IR_LOAD,                                    -- ObjectKind=Pin|PrimaryId=U6-C
        D => PWM_D,                                          -- ObjectKind=Pin|PrimaryId=U6-D[7..0]
        Q => PinSignal_U6_Q                                  -- ObjectKind=Pin|PrimaryId=U6-Q[7..0]
      );

    U5 : CB8CLEDB                                            -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U2_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U5-C
        CE  => PinSignal_U29_O,                              -- ObjectKind=Pin|PrimaryId=U5-CE
        CLR => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U5-CLR
        D   => PinSignal_U6_Q,                               -- ObjectKind=Pin|PrimaryId=U5-D[7..0]
        L   => PinSignal_U3_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U5-L
        TC  => PinSignal_U5_TC,                              -- ObjectKind=Pin|PrimaryId=U5-TC
        UP  => PowerSignal_GND                               -- ObjectKind=Pin|PrimaryId=U5-UP
      );

    U4 : INV                                                 -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      Port Map
      (
        I => PinSignal_U3_CLKDV,                             -- ObjectKind=Pin|PrimaryId=U4-I
        O => PinSignal_U4_O                                  -- ObjectKind=Pin|PrimaryId=U4-O
      );

    U3 : CDIV256                                             -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U3_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN => PinSignal_U2_CLKDV                          -- ObjectKind=Pin|PrimaryId=U3-CLKIN
      );

    U2 : CDIV128DC50                                         -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U2_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U2-CLKDV
        CLKIN => CLK_20MHZ                                   -- ObjectKind=Pin|PrimaryId=U2-CLKIN
      );

    U1 : FDC                                                 -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U4_O,                               -- ObjectKind=Pin|PrimaryId=U1-C
        CLR => PinSignal_U5_TC,                              -- ObjectKind=Pin|PrimaryId=U1-CLR
        D   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U1-D
        Q   => PinSignal_U1_Q                                -- ObjectKind=Pin|PrimaryId=U1-Q
      );

    -- Signal Assignments
    ---------------------
    IN1             <= PinSignal_U14_Q0; -- ObjectKind=Net|PrimaryId=NetU14_Q0
    IN2             <= PinSignal_U14_Q1; -- ObjectKind=Net|PrimaryId=NetU14_Q1
    IR_STRIP        <= PinSignal_U22_O; -- ObjectKind=Net|PrimaryId=NetU22_O
    OK_TO_MOVE      <= PinSignal_U17_O; -- ObjectKind=Net|PrimaryId=NetU15_I1
    PowerSignal_GND <= '0'; -- ObjectKind=Net|PrimaryId=GND
    PowerSignal_VCC <= '1'; -- ObjectKind=Net|PrimaryId=VCC
    PWM_IR_OUT      <= PinSignal_U23_Q; -- ObjectKind=Net|PrimaryId=NetU23_Q
    PWM_UV_OUT      <= PinSignal_U25_Q; -- ObjectKind=Net|PrimaryId=NetU25_Q
    PWM_WHT_OUT     <= PinSignal_U27_Q; -- ObjectKind=Net|PrimaryId=NetU27_Q
    X_HOME          <= PinSignal_U21_O; -- ObjectKind=Net|PrimaryId=NetU21_O

end structure;
------------------------------------------------------------


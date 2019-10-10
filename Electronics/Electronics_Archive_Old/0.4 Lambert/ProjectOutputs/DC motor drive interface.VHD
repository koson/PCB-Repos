------------------------------------------------------------
-- VHDL DC_motor_drive_interface
-- 2009 9 21 7 15 49
-- Created By "DXP VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL DC_motor_drive_interface
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

--synthesis translate_off
Library GENERIC_LIB;
Use     GENERIC_LIB.all;

--synthesis translate_on
Entity DC_motor_drive_interface Is
  port
  (
    LEDS0 : Out   STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=LEDS0
    LEDS1 : Out   STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=LEDS1
    LEDS2 : Out   STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=LEDS2
    PB0   : In    STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=PB0
    PB1   : In    STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=PB1
    PB2   : In    STD_LOGIC;                                 -- ObjectKind=Port|PrimaryId=PB2
    PB3   : In    STD_LOGIC                                  -- ObjectKind=Port|PrimaryId=PB3
  );
  attribute MacroCell : boolean;

End DC_motor_drive_interface;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of DC_motor_drive_interface is
   Component AND2S                                           -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U5-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U5-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U5-O
      );
   End Component;

   Component FD2CS                                           -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-C
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-CLR
        D0  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-D0
        D1  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-D1
        Q0  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-Q0
        Q1  : out STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U4-Q1
      );
   End Component;

   Component INV                                             -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      port
      (
        I : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U1-I
        O : out STD_LOGIC                                    -- ObjectKind=Pin|PrimaryId=U1-O
      );
   End Component;

   Component OR2S                                            -- ObjectKind=Part|PrimaryId=U7|SecondaryId=1
      port
      (
        I0 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U7-I0
        I1 : in  STD_LOGIC;                                  -- ObjectKind=Pin|PrimaryId=U7-I1
        O  : out STD_LOGIC                                   -- ObjectKind=Pin|PrimaryId=U7-O
      );
   End Component;


    Signal PinSignal_U1_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_O
    Signal PinSignal_U10_O : STD_LOGIC; -- ObjectKind=Net|PrimaryId=Netu9_I0
    Signal PinSignal_U2_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU2_O
    Signal PinSignal_U3_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU3_O
    Signal PinSignal_U4_Q0 : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_I
    Signal PinSignal_U4_Q1 : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU2_I
    Signal PinSignal_U5_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU5_O
    Signal PinSignal_U6_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU6_O
    Signal PinSignal_U7_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU4_CLR
    Signal PinSignal_U8_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU5_I0
    Signal PinSignal_u9_O  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU7_I0

begin
    U10 : INV                                                -- ObjectKind=Part|PrimaryId=U10|SecondaryId=1
      Port Map
      (
        I => PB3,                                            -- ObjectKind=Pin|PrimaryId=U10-I
        O => PinSignal_U10_O                                 -- ObjectKind=Pin|PrimaryId=U10-O
      );

    u9 : AND2S                                               -- ObjectKind=Part|PrimaryId=u9|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U10_O,                               -- ObjectKind=Pin|PrimaryId=u9-I0
        I1 => PinSignal_U4_Q1,                               -- ObjectKind=Pin|PrimaryId=u9-I1
        O  => PinSignal_u9_O                                 -- ObjectKind=Pin|PrimaryId=u9-O
      );

    U8 : INV                                                 -- ObjectKind=Part|PrimaryId=U8|SecondaryId=1
      Port Map
      (
        I => PB2,                                            -- ObjectKind=Pin|PrimaryId=U8-I
        O => PinSignal_U8_O                                  -- ObjectKind=Pin|PrimaryId=U8-O
      );

    U7 : OR2S                                                -- ObjectKind=Part|PrimaryId=U7|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_u9_O,                                -- ObjectKind=Pin|PrimaryId=U7-I0
        I1 => PinSignal_U5_O,                                -- ObjectKind=Pin|PrimaryId=U7-I1
        O  => PinSignal_U7_O                                 -- ObjectKind=Pin|PrimaryId=U7-O
      );

    U6 : INV                                                 -- ObjectKind=Part|PrimaryId=U6|SecondaryId=1
      Port Map
      (
        I => PinSignal_U7_O,                                 -- ObjectKind=Pin|PrimaryId=U6-I
        O => PinSignal_U6_O                                  -- ObjectKind=Pin|PrimaryId=U6-O
      );

    U5 : AND2S                                               -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      Port Map
      (
        I0 => PinSignal_U8_O,                                -- ObjectKind=Pin|PrimaryId=U5-I0
        I1 => PinSignal_U4_Q0,                               -- ObjectKind=Pin|PrimaryId=U5-I1
        O  => PinSignal_U5_O                                 -- ObjectKind=Pin|PrimaryId=U5-O
      );

    U4 : FD2CS                                               -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      Port Map
      (
        C   => PB1,                                          -- ObjectKind=Pin|PrimaryId=U4-C
        CLR => PinSignal_U7_O,                               -- ObjectKind=Pin|PrimaryId=U4-CLR
        D0  => PinSignal_U3_O,                               -- ObjectKind=Pin|PrimaryId=U4-D0
        D1  => PB0,                                          -- ObjectKind=Pin|PrimaryId=U4-D1
        Q0  => PinSignal_U4_Q0,                              -- ObjectKind=Pin|PrimaryId=U4-Q0
        Q1  => PinSignal_U4_Q1                               -- ObjectKind=Pin|PrimaryId=U4-Q1
      );

    U3 : INV                                                 -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      Port Map
      (
        I => PB0,                                            -- ObjectKind=Pin|PrimaryId=U3-I
        O => PinSignal_U3_O                                  -- ObjectKind=Pin|PrimaryId=U3-O
      );

    U2 : INV                                                 -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      Port Map
      (
        I => PinSignal_U4_Q1,                                -- ObjectKind=Pin|PrimaryId=U2-I
        O => PinSignal_U2_O                                  -- ObjectKind=Pin|PrimaryId=U2-O
      );

    U1 : INV                                                 -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      Port Map
      (
        I => PinSignal_U4_Q0,                                -- ObjectKind=Pin|PrimaryId=U1-I
        O => PinSignal_U1_O                                  -- ObjectKind=Pin|PrimaryId=U1-O
      );

    -- Signal Assignments
    ---------------------
    LEDS0 <= PinSignal_U1_O; -- ObjectKind=Net|PrimaryId=NetU1_O
    LEDS1 <= PinSignal_U2_O; -- ObjectKind=Net|PrimaryId=NetU2_O
    LEDS2 <= PinSignal_U6_O; -- ObjectKind=Net|PrimaryId=NetU6_O

end structure;
------------------------------------------------------------


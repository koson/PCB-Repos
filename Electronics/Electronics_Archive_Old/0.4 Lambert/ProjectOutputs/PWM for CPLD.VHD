------------------------------------------------------------
-- VHDL PWM_for_CPLD
-- 2009 9 16 10 29 32
-- Created By "DXP VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL PWM_for_CPLD
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

--synthesis translate_off
Library GENERIC_LIB;
Use     GENERIC_LIB.all;

--synthesis translate_on
Entity PWM_for_CPLD Is
  port
  (
    CLK : In    STD_LOGIC;                                   -- ObjectKind=Port|PrimaryId=CLK
    HA1 : Out   STD_LOGIC;                                   -- ObjectKind=Port|PrimaryId=HA1
    HA2 : Out   STD_LOGIC;                                   -- ObjectKind=Port|PrimaryId=HA2
    HA3 : Out   STD_LOGIC;                                   -- ObjectKind=Port|PrimaryId=HA3
    HA4 : Out   STD_LOGIC;                                   -- ObjectKind=Port|PrimaryId=HA4
    SW  : In    STD_LOGIC_VECTOR(7 DOWNTO 0)                 -- ObjectKind=Port|PrimaryId=SW[7..0]
  );
  attribute MacroCell : boolean;

End PWM_for_CPLD;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of PWM_for_CPLD is
   Component CB8CLEDB                                        -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-C
        CE  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-CE
        CEO : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-CEO
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-CLR
        D   : in  STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U4-D[7..0]
        L   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-L
        Q   : out STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U4-Q[7..0]
        TC  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U4-TC
        UP  : in  STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U4-UP
      );
   End Component;

   Component CDIV8DC50                                       -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U5-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U5-CLKIN
      );
   End Component;

   Component CDIV10DC50                                      -- ObjectKind=Part|PrimaryId=U6|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U6-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U6-CLKIN
      );
   End Component;

   Component CDIV256                                         -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U3-CLKIN
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

   Component INV                                             -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      port
      (
        I : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U2-I
        O : out STD_LOGIC                                    -- ObjectKind=Pin|PrimaryId=U2-O
      );
   End Component;


    Signal NamedSignal_250_HZ : STD_LOGIC; -- ObjectKind=Net|PrimaryId=250_HZ
    Signal PinSignal_U1_Q     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_Q
    Signal PinSignal_U2_O     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU2_O
    Signal PinSignal_U20_O    : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU20_O
    Signal PinSignal_U3_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_C
    Signal PinSignal_U4_TC    : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU1_CLR
    Signal PinSignal_U5_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=2.5MHZ
    Signal PinSignal_U6_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU6_CLKDV
    Signal PinSignal_U7_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU7_CLKDV
    Signal PinSignal_U8_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU8_CLKDV
    Signal PinSignal_U9_CLKDV : STD_LOGIC; -- ObjectKind=Net|PrimaryId=250_HZ
    Signal PowerSignal_GND    : STD_LOGIC; -- ObjectKind=Net|PrimaryId=GND
    Signal PowerSignal_VCC    : STD_LOGIC; -- ObjectKind=Net|PrimaryId=VCC

begin
    U20 : INV                                                -- ObjectKind=Part|PrimaryId=U20|SecondaryId=1
      Port Map
      (
        I => PinSignal_U4_TC,                                -- ObjectKind=Pin|PrimaryId=U20-I
        O => PinSignal_U20_O                                 -- ObjectKind=Pin|PrimaryId=U20-O
      );

    U9 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U9|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U9_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U9-CLKDV
        CLKIN => PinSignal_U8_CLKDV                          -- ObjectKind=Pin|PrimaryId=U9-CLKIN
      );

    U8 : CDIV10DC50                                          -- ObjectKind=Part|PrimaryId=U8|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U8_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U8-CLKDV
        CLKIN => PinSignal_U7_CLKDV                          -- ObjectKind=Pin|PrimaryId=U8-CLKIN
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

    U5 : CDIV8DC50                                           -- ObjectKind=Part|PrimaryId=U5|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U5_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U5-CLKDV
        CLKIN => CLK                                         -- ObjectKind=Pin|PrimaryId=U5-CLKIN
      );

    U4 : CB8CLEDB                                            -- ObjectKind=Part|PrimaryId=U4|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U2_O,                               -- ObjectKind=Pin|PrimaryId=U4-C
        CE  => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U4-CE
        CLR => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U4-CLR
        D   => SW,                                           -- ObjectKind=Pin|PrimaryId=U4-D[7..0]
        L   => PinSignal_U3_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U4-L
        TC  => PinSignal_U4_TC,                              -- ObjectKind=Pin|PrimaryId=U4-TC
        UP  => PowerSignal_GND                               -- ObjectKind=Pin|PrimaryId=U4-UP
      );

    U3 : CDIV256                                             -- ObjectKind=Part|PrimaryId=U3|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U3_CLKDV,                         -- ObjectKind=Pin|PrimaryId=U3-CLKDV
        CLKIN => NamedSignal_250_HZ                          -- ObjectKind=Pin|PrimaryId=U3-CLKIN
      );

    U2 : INV                                                 -- ObjectKind=Part|PrimaryId=U2|SecondaryId=1
      Port Map
      (
        I => NamedSignal_250_HZ,                             -- ObjectKind=Pin|PrimaryId=U2-I
        O => PinSignal_U2_O                                  -- ObjectKind=Pin|PrimaryId=U2-O
      );

    U1 : FDC                                                 -- ObjectKind=Part|PrimaryId=U1|SecondaryId=1
      Port Map
      (
        C   => PinSignal_U3_CLKDV,                           -- ObjectKind=Pin|PrimaryId=U1-C
        CLR => PinSignal_U4_TC,                              -- ObjectKind=Pin|PrimaryId=U1-CLR
        D   => PowerSignal_VCC,                              -- ObjectKind=Pin|PrimaryId=U1-D
        Q   => PinSignal_U1_Q                                -- ObjectKind=Pin|PrimaryId=U1-Q
      );

    -- Signal Assignments
    ---------------------
    HA1                <= PinSignal_U3_CLKDV; -- ObjectKind=Net|PrimaryId=NetU1_C
    HA2                <= PinSignal_U4_TC; -- ObjectKind=Net|PrimaryId=NetU1_CLR
    HA3                <= PinSignal_U1_Q; -- ObjectKind=Net|PrimaryId=NetU1_Q
    HA4                <= PinSignal_U20_O; -- ObjectKind=Net|PrimaryId=NetU20_O
    NamedSignal_250_HZ <= PinSignal_U9_CLKDV; -- ObjectKind=Net|PrimaryId=250_HZ
    PowerSignal_GND    <= '0'; -- ObjectKind=Net|PrimaryId=GND
    PowerSignal_VCC    <= '1'; -- ObjectKind=Net|PrimaryId=VCC

end structure;
------------------------------------------------------------


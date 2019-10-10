------------------------------------------------------------
-- VHDL Indexer
-- 2009 9 21 7 15 49
-- Created By "DXP VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL Indexer
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

--synthesis translate_off
Library GENERIC_LIB;
Use     GENERIC_LIB.all;

--synthesis translate_on
Entity Indexer Is
  port
  (
    CLK  : In    STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=CLK
    LEDS : Out   STD_LOGIC_VECTOR(7 DOWNTO 0);               -- ObjectKind=Port|PrimaryId=LEDS[7..0]
    PB0  : In    STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=PB0
    PB1  : In    STD_LOGIC                                   -- ObjectKind=Port|PrimaryId=PB1
  );
  attribute MacroCell : boolean;

End Indexer;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of Indexer is
   Component CB8CLEDB                                        -- ObjectKind=Part|PrimaryId=U16|SecondaryId=1
      port
      (
        C   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-C
        CE  : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-CE
        CEO : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-CEO
        CLR : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-CLR
        D   : in  STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U16-D[7..0]
        L   : in  STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-L
        Q   : out STD_LOGIC_VECTOR(7 downto 0);              -- ObjectKind=Pin|PrimaryId=U16-Q[7..0]
        TC  : out STD_LOGIC;                                 -- ObjectKind=Pin|PrimaryId=U16-TC
        UP  : in  STD_LOGIC                                  -- ObjectKind=Pin|PrimaryId=U16-UP
      );
   End Component;

   Component CDIV10DC50                                      -- ObjectKind=Part|PrimaryId=U10|SecondaryId=1
      port
      (
        CLKDV : out STD_LOGIC;                               -- ObjectKind=Pin|PrimaryId=U10-CLKDV
        CLKIN : in  STD_LOGIC                                -- ObjectKind=Pin|PrimaryId=U10-CLKIN
      );
   End Component;

   Component INV                                             -- ObjectKind=Part|PrimaryId=U17|SecondaryId=1
      port
      (
        I : in  STD_LOGIC;                                   -- ObjectKind=Pin|PrimaryId=U17-I
        O : out STD_LOGIC                                    -- ObjectKind=Pin|PrimaryId=U17-O
      );
   End Component;


    Signal NamedSignal_20HZ     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=20HZ
    Signal NamedSignal_GND1_BUS : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=GND1_BUS[7..0]
    Signal PinSignal_U10_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU10_CLKDV
    Signal PinSignal_U11_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU11_CLKDV
    Signal PinSignal_U12_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU12_CLKDV
    Signal PinSignal_U13_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU13_CLKDV
    Signal PinSignal_U14_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU14_CLKDV
    Signal PinSignal_U15_CLKDV  : STD_LOGIC; -- ObjectKind=Net|PrimaryId=20HZ
    Signal PinSignal_U16_Q      : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=LEDS[7..0]
    Signal PinSignal_U16_TC     : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU16_TC
    Signal PinSignal_U17_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU16_CE
    Signal PinSignal_U18_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU16_UP
    Signal PinSignal_U19_O      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=NetU16_CLR
    Signal PowerSignal_GND      : STD_LOGIC; -- ObjectKind=Net|PrimaryId=GND

begin
    U19 : INV                                                -- ObjectKind=Part|PrimaryId=U19|SecondaryId=1
      Port Map
      (
        I => PB1,                                            -- ObjectKind=Pin|PrimaryId=U19-I
        O => PinSignal_U19_O                                 -- ObjectKind=Pin|PrimaryId=U19-O
      );

    U18 : INV                                                -- ObjectKind=Part|PrimaryId=U18|SecondaryId=1
      Port Map
      (
        I => PB0,                                            -- ObjectKind=Pin|PrimaryId=U18-I
        O => PinSignal_U18_O                                 -- ObjectKind=Pin|PrimaryId=U18-O
      );

    U17 : INV                                                -- ObjectKind=Part|PrimaryId=U17|SecondaryId=1
      Port Map
      (
        I => PinSignal_U16_TC,                               -- ObjectKind=Pin|PrimaryId=U17-I
        O => PinSignal_U17_O                                 -- ObjectKind=Pin|PrimaryId=U17-O
      );

    U16 : CB8CLEDB                                           -- ObjectKind=Part|PrimaryId=U16|SecondaryId=1
      Port Map
      (
        C   => NamedSignal_20HZ,                             -- ObjectKind=Pin|PrimaryId=U16-C
        CE  => PinSignal_U17_O,                              -- ObjectKind=Pin|PrimaryId=U16-CE
        CLR => PinSignal_U19_O,                              -- ObjectKind=Pin|PrimaryId=U16-CLR
        D   => NamedSignal_GND1_BUS,                         -- ObjectKind=Pin|PrimaryId=U16-D[7..0]
        L   => PowerSignal_GND,                              -- ObjectKind=Pin|PrimaryId=U16-L
        Q   => PinSignal_U16_Q,                              -- ObjectKind=Pin|PrimaryId=U16-Q[7..0]
        TC  => PinSignal_U16_TC,                             -- ObjectKind=Pin|PrimaryId=U16-TC
        UP  => PinSignal_U18_O                               -- ObjectKind=Pin|PrimaryId=U16-UP
      );

    U15 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U15|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U15_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U15-CLKDV
        CLKIN => PinSignal_U14_CLKDV                         -- ObjectKind=Pin|PrimaryId=U15-CLKIN
      );

    U14 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U14|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U14_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U14-CLKDV
        CLKIN => PinSignal_U13_CLKDV                         -- ObjectKind=Pin|PrimaryId=U14-CLKIN
      );

    U13 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U13|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U13_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U13-CLKDV
        CLKIN => PinSignal_U12_CLKDV                         -- ObjectKind=Pin|PrimaryId=U13-CLKIN
      );

    U12 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U12|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U12_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U12-CLKDV
        CLKIN => PinSignal_U11_CLKDV                         -- ObjectKind=Pin|PrimaryId=U12-CLKIN
      );

    U11 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U11|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U11_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U11-CLKDV
        CLKIN => PinSignal_U10_CLKDV                         -- ObjectKind=Pin|PrimaryId=U11-CLKIN
      );

    U10 : CDIV10DC50                                         -- ObjectKind=Part|PrimaryId=U10|SecondaryId=1
      Port Map
      (
        CLKDV => PinSignal_U10_CLKDV,                        -- ObjectKind=Pin|PrimaryId=U10-CLKDV
        CLKIN => CLK                                         -- ObjectKind=Pin|PrimaryId=U10-CLKIN
      );

    -- Signal Assignments
    ---------------------
    LEDS                 <= PinSignal_U16_Q; -- ObjectKind=Net|PrimaryId=LEDS[7..0]
    NamedSignal_20HZ     <= PinSignal_U15_CLKDV; -- ObjectKind=Net|PrimaryId=20HZ
    NamedSignal_GND1_BUS <= "00000000"; -- ObjectKind=Net|PrimaryId=GND1_BUS[7..0]
    PowerSignal_GND      <= '0'; -- ObjectKind=Net|PrimaryId=GND

end structure;
------------------------------------------------------------


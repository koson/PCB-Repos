------------------------------------------------------------
-- VHDL Top_Level_1
-- 2009 9 16 10 56 29
-- Created By "Altium Designer VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL Top_Level_1
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

Entity PWM_generator Is
  port
  (
    CLK  : In    STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=CLK
    HA1  : Out   STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=HA1
    HA2  : Out   STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=HA2
    HA3  : Out   STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=HA3
    HA4  : Out   STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=HA4
    LEDS : Out   STD_LOGIC_VECTOR(7 DOWNTO 0);               -- ObjectKind=Port|PrimaryId=LEDS[7..0]
    PB0  : In    STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=PB0
    PB1  : In    STD_LOGIC;                                  -- ObjectKind=Port|PrimaryId=PB1
    SW   : In    STD_LOGIC_VECTOR(7 DOWNTO 0)                -- ObjectKind=Port|PrimaryId=SW[7..0]
  );
  attribute MacroCell : boolean;

  attribute FPGA_CLOCK : string;
  attribute FPGA_CLOCK of CLK : Signal is "True";

  attribute FPGA_PINNUM : string;
  attribute FPGA_PINNUM of CLK  : Signal is "P55";
  attribute FPGA_PINNUM of HA1  : Signal is "P27";
  attribute FPGA_PINNUM of HA2  : Signal is "P28";
  attribute FPGA_PINNUM of HA3  : Signal is "P29";
  attribute FPGA_PINNUM of HA4  : Signal is "P30";
  attribute FPGA_PINNUM of LEDS : Signal is "P175,P178,P179,P180,P182,P183,P184,P185";
  attribute FPGA_PINNUM of PB0  : Signal is "P127";
  attribute FPGA_PINNUM of PB1  : Signal is "P126";
  attribute FPGA_PINNUM of SW   : Signal is "P40,P41,P43,P45,P47,P48,P49,P50";


End PWM_generator;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of PWM_generator is
   Component Indexer                                         -- ObjectKind=Sheet Symbol|PrimaryId=Indexer_1
      port
      (
        CLK  : in  STD_LOGIC;                                -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-CLK
        LEDS : out STD_LOGIC_VECTOR(7 downto 0);             -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-LEDS[7..0]
        PB0  : in  STD_LOGIC;                                -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB0
        PB1  : in  STD_LOGIC                                 -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB1
      );
   End Component;

   Component PWM_for_CPLD                                    -- ObjectKind=Sheet Symbol|PrimaryId=PWM
      port
      (
        CLK : in  STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-CLK
        HA1 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA1
        HA2 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA2
        HA3 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA3
        HA4 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA4
        SW  : in  STD_LOGIC_VECTOR(7 downto 0)               -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-SW[7..0]
      );
   End Component;


    Signal NamedSignal_CLK          : STD_LOGIC; -- ObjectKind=Net|PrimaryId=CLK
    Signal PinSignal_Indexer_1_LEDS : STD_LOGIC_VECTOR(7 downto 0); -- ObjectKind=Net|PrimaryId=LEDS[7..0]
    Signal PinSignal_PWM_HA1        : STD_LOGIC; -- ObjectKind=Net|PrimaryId=HA1
    Signal PinSignal_PWM_HA2        : STD_LOGIC; -- ObjectKind=Net|PrimaryId=HA2
    Signal PinSignal_PWM_HA3        : STD_LOGIC; -- ObjectKind=Net|PrimaryId=HA3
    Signal PinSignal_PWM_HA4        : STD_LOGIC; -- ObjectKind=Net|PrimaryId=HA4



begin
    PWM : PWM_for_CPLD                                       -- ObjectKind=Sheet Symbol|PrimaryId=PWM
      Port Map
      (
        CLK => CLK,                                          -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-CLK
        HA1 => PinSignal_PWM_HA1,                            -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA1
        HA2 => PinSignal_PWM_HA2,                            -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA2
        HA3 => PinSignal_PWM_HA3,                            -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA3
        HA4 => PinSignal_PWM_HA4,                            -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA4
        SW  => SW                                            -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-SW[7..0]
      );

    Indexer_1 : Indexer                                      -- ObjectKind=Sheet Symbol|PrimaryId=Indexer_1
      Port Map
      (
        CLK  => NamedSignal_CLK,                             -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-CLK
        LEDS => PinSignal_Indexer_1_LEDS,                    -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-LEDS[7..0]
        PB0  => PB0,                                         -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB0
        PB1  => PB1                                          -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB1
      );

    -- Signal Assignments
    ---------------------
    HA1             <= PinSignal_PWM_HA1; -- ObjectKind=Net|PrimaryId=HA1
    HA2             <= PinSignal_PWM_HA2; -- ObjectKind=Net|PrimaryId=HA2
    HA3             <= PinSignal_PWM_HA3; -- ObjectKind=Net|PrimaryId=HA3
    HA4             <= PinSignal_PWM_HA4; -- ObjectKind=Net|PrimaryId=HA4
    LEDS            <= PinSignal_Indexer_1_LEDS; -- ObjectKind=Net|PrimaryId=LEDS[7..0]
    NamedSignal_CLK <= CLK; -- ObjectKind=Net|PrimaryId=CLK

end structure;
------------------------------------------------------------


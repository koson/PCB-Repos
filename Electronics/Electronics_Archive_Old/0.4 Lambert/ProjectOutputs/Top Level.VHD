------------------------------------------------------------
-- VHDL Top_Level
-- 2009 9 15 14 56 5
-- Created By "DXP VHDL Generator"
-- "Copyright (c) 2002-2004 Altium Limited"
------------------------------------------------------------

------------------------------------------------------------
-- VHDL Top_Level
------------------------------------------------------------

Library IEEE;
Use     IEEE.std_logic_1164.all;

Entity Top_Level Is
  attribute MacroCell : boolean;

End Top_Level;
------------------------------------------------------------

------------------------------------------------------------
architecture structure of Top_Level is
   Component Indexer                                         -- ObjectKind=Sheet Symbol|PrimaryId=Indexer
      port
      (
        LEDS : out STD_LOGIC_VECTOR(7 downto 0);             -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-LEDS[7..0]
        PB0  : in  STD_LOGIC;                                -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB0
        PB1  : in  STD_LOGIC                                 -- ObjectKind=Sheet Entry|PrimaryId=Indexer.SchDoc-PB1
      );
   End Component;

   Component PWM_for_CPLD                                    -- ObjectKind=Sheet Symbol|PrimaryId=PWM
      port
      (
        HA1 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA1
        HA2 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA2
        HA3 : out STD_LOGIC;                                 -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-HA3
        SW  : in  STD_LOGIC_VECTOR(7 downto 0)               -- ObjectKind=Sheet Entry|PrimaryId=PWM for CPLD.SchDoc-SW[7..0]
      );
   End Component;



begin
    PWM : PWM_for_CPLD                                       -- ObjectKind=Sheet Symbol|PrimaryId=PWM
;

    IndexerAUKWJPUG : Indexer                                -- ObjectKind=Sheet Symbol|PrimaryId=Indexer
;

end structure;
------------------------------------------------------------


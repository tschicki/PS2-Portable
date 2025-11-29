
library ieee;
use ieee.std_logic_1164.all;

package ram_pkg is
    function logb2 (depth: in natural) return integer;
end ram_pkg;

package body ram_pkg is

function logb2( depth : natural) return integer is
--variable temp    : integer := depth;
--variable ret_val : integer := 0;
--begin
--    while temp > 1 loop
--        ret_val := ret_val + 1;
--        temp    := temp / 2;
--    end loop;

--    return ret_val;
variable temp    : integer := 0;
variable ret_val : integer := 0;
begin
    while temp < depth loop
        ret_val := ret_val + 1;
        temp := 2**ret_val;
    end loop;

    return ret_val;
end function;

end package body ram_pkg;


library ieee;
library work;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.ram_pkg.all;
use std.textio.all;

entity efx_simple_dual_port_2_clk_ram is
generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer :=2048;
    OUTREG    : boolean := true             -- Set OUTREG true or false. 
    );

port (
  clka  : in std_logic;                       		     -- Write Clock
  clkb  : in std_logic;                       		     -- Read Clock
  addra : in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);  -- Write address
  addrb : in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);  -- Read address 
  dina  : in std_logic_vector(RAM_WIDTH-1 downto 0);	     -- RAM input data
  wea   : in std_logic;                       		     -- Write enable
  enb   : in std_logic;                       		     -- RAM Enable
  rstb  : in std_logic;                       		     -- Output reset 
  regceb: in std_logic;                       		     -- Output register enable
  doutb : out std_logic_vector(RAM_WIDTH-1 downto 0)   	     -- RAM output data
  );

end efx_simple_dual_port_2_clk_ram;

architecture rtl of efx_simple_dual_port_2_clk_ram is

constant C_RAM_WIDTH : integer := RAM_WIDTH;
constant C_RAM_DEPTH : integer := RAM_DEPTH;
constant C_OUTREG    : boolean := OUTREG;

signal doutb_reg : std_logic_vector(C_RAM_WIDTH-1 downto 0) := (others => '0');
type ram_type is array (C_RAM_DEPTH-1 downto 0) of std_logic_vector (C_RAM_WIDTH-1 downto 0);        
signal ram_data : std_logic_vector(C_RAM_WIDTH-1 downto 0) ;
signal ram_name : ram_type;


begin

process(clka)
begin
    if(rising_edge(clka)) then
        if(wea = '1') then
            ram_name(to_integer(unsigned(addra))) <= dina;
        end if;
    end if;
end process;

process(clkb)
begin
    if(rising_edge(clkb)) then
        if(enb = '1') then
            ram_data <= ram_name(to_integer(unsigned(addrb)));
        end if;
    end if;
end process;


no_outreg : if not C_OUTREG  generate
    doutb <= ram_data;
end generate;


yes_outreg : if C_OUTREG  generate
process(clkb)
begin
    if(rising_edge(clkb)) then
        if(rstb = '0') then
            doutb_reg <= (others => '0');
        elsif(regceb = '1') then
            doutb_reg <= ram_data;
        end if;
    end if;
end process;
doutb <= doutb_reg;

end generate;
end rtl;



## Copyright (C) 2009 Onno Kortmann <onno@gmx.net>
##  
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##  
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##  
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##  
{
#if $has_spi
    ## FIXME: care about mega spi flag!
    spi=new HWSpi(this,
		  irqSystem,
		  PinAtPort(port$pkg_portlet["MOSI"],$pkg_portbit["MOSI"]),
		  PinAtPort(port$pkg_portlet["MISO"],$pkg_portbit["MISO"]),
		  PinAtPort(port$pkg_portlet["SCK"],$pkg_portbit["SCK"]),
		  PinAtPort(port$pkg_portlet["'SS"],$pkg_portbit["'SS"]),
		  $irq_bysrc["SPI, STC"].addr, true);

    rw[$io["SPCR"].addr]=& spi->spcr_reg;
    rw[$io["SPSR"].addr]=& spi->spsr_reg;
    rw[$io["SPDR"].addr]=& spi->spdr_reg;
#endif						
}

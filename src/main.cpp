/*
    This file is part of GlakkeClock.

    GlakkeClock, ATI GPU Overclock.
    Copyright (C) 2010-2011  Andrée Ekroth

    GlakkeClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    GlakkeClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GlakkeClock.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GlakkeClock.hpp"

int main(int argc, char **argv)
{
	kke::GlakkeClock glakkeClock;

	return glakkeClock.Execute(argc, argv);
}

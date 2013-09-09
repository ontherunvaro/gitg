/*
 * This file is part of gitg
 *
 * Copyright (C) 2013 - Sindhu S
 *
 * gitg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gitg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gitg. If not, see <http://www.gnu.org/licenses/>.
 */


namespace Gitg
{

public class RebaseWindow : Gtk.Window
{
	public RebaseWindow()
	{
		this.title = "gitg Rebase";
		destroy.connect (Gtk.main_quit);
		var rebase_listbox = new RebaseListBox();
		rebase_listbox.add_rebase_row("sha1", "msg");
		rebase_listbox.add_rebase_row("sha2", "msg");
		rebase_listbox.add_rebase_row("sha3", "msg");
		var hbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 1);
		hbox.homogeneous = true;
		hbox.add (rebase_listbox);
		add (hbox);
	}
}

}
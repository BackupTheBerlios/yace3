/*
  Copyright (C) 2004 Thomas Yao

  This file is part of YaCE 2

  YaCE 2 is free software, you can redistribute it and/or
  modify it under the terms of the Affero General Public License as
  published by Affero, Inc., either version 1 of the License, or
  (at your option) any later version.

  YaCE 2 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY, without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  Affero General Public License for more details.

  You should have received a copy of the Affero General Public
  License in the COPYING file that comes with YaCE2. If
  not, write to Affero, Inc., 510 Third Street, Suite 225, San
  Francisco, CA 94107 USA.
*/

package gogi.yace2.regconv;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.util.Enumeration;
import java.util.Hashtable;

public class RegConverter
{
	private String m_dir;
	private int m_format;

	public RegConverter(String dir, int format)
	{
		m_dir = dir;
		m_format = format;
	}

	public boolean convert()
	{
		// set up the users hashtable
		Hashtable users = new Hashtable();

		File dir;
		File dir1;

		// set up the two directories
		dir = new File(m_dir);
		dir1 = new File(m_dir + "old");

		// check if dir1 exists, if yes delete it
		if(!dir.renameTo(dir1))
		{
			System.out.println(dir1.getAbsolutePath() + " already exists! Deleting...");
			if(!deleteFolder(dir1) && !dir1.delete())
			{
				System.out.println("Deleting failed!");
				return false;
			}
			else
				dir.renameTo(dir1);
		}

		// rename dir to dir1 and create a new dir
		dir.mkdir();

		// check for right permissions
		if(!dir1.exists() || !dir1.isDirectory() || !dir1.canRead() || !dir1.canWrite())
		{
			System.out.println("Permission denied to " + dir1.getAbsolutePath() + ": Please check if you have the right permissions.");
			return false;
		}

		// get file list
		String reglist[] = dir1.list();

		// declare all user variables
		String name;
		String password;
		long rights;
		String color;
		String lastlogin;
		Hashtable props;
		String line;

		for(int i = 0; i < reglist.length; ++i)
		{
			String file = reglist[i];
			System.out.println("Processing " + file + "...");
			FileInputStream fileinputstream = null;
			try
			{
				fileinputstream = new FileInputStream(dir1.getAbsolutePath() + "/" + file);
			}
			catch(java.io.FileNotFoundException e)
			{
				System.out.println("Error opening " + file + "! Continue with next file...");
				continue;
			}
			BufferedReader bufferedreader = new BufferedReader(new InputStreamReader(fileinputstream));

			name = "";
			password = "";
			rights = 0;
			color = "";
			lastlogin = "";
			props = new Hashtable();
			line = "";

			// read in name and password
			name = readLine(bufferedreader);
			password = readLine(bufferedreader);

			// check for null pointer
			if(name == null || password == null)
			{
				System.out.println("Damaged file " + file + " - skipping...");
				continue;
			}

			// read in next line
			line = readLine(bufferedreader);

			// check for null pointer
			if(line == null)
			{
				System.out.println("Damaged file " + file + " - skipping...");
				continue;
			}

			// to do, if yace 1 file format
			if(m_format == 1)
			{
				// read in next line (rights)
				try
				{
					rights = Long.parseLong(line);
				}
				catch(NumberFormatException e)
				{
					rights = 0;
				}

				// read in next lines
				color = readLine(bufferedreader);
				lastlogin = readLine(bufferedreader);

				// read in next line ("--REGITEMS--")
				line = readLine(bufferedreader);

				// check for null pointer
				if(color == null || lastlogin == null || line == null)
				{
					System.out.println("Damaged file " + file + " - skipping...");
					continue;
				}

				// check if line is "--REGITEMS--"
				if(!line.equals("--REGITEMS--"))
				{
					System.out.println("Damaged file " + file + " - skipping...");
					continue;
				}

				// read until file end
				while((line = readLine(bufferedreader)) != null)
				{
					// split the line by "="
					String[] split = line.split("=", 2);

					// check if array size is right
					if(split.length != 2)
					{
						System.out.println("Damaged file " + file + " - skipping...");
						continue;
					}

					// put the regitem into the props hashtable
					props.put("reginfo_" + split[0], split[1]);
				}
			}
			// to do, if yace 2 file format
			else
			{
				// read next line
				line = readLine(bufferedreader);

				// check for null pointer and if string equals "---REGITEMS---"
				if(line == null || !line.equals("---REGITEMS---"))
				{
					System.out.println("Damaged file " + file + " - skipping...");
					continue;
				}

				// read until file end
				while((line = readLine(bufferedreader)) != null)
				{
					// split the line by "="
					String[] split = line.split("=", 2);
					if(split.length != 2)
					{
						System.out.println("Damaged file " + file + " - skipping...");
						continue;
					}

					// if a reginfo was found
					if(split[0].startsWith("reg_") && !split[0].equals("reg_activated"))
					{
						// cut the " out of the second part of the line
						// replace the "reg_" with "reginfo_"
						props.put("reginfo_" + split[0].substring(4), split[1].substring(1, split[1].length() - 1));
					}
					// if regitem rights was found
					else if(split[0].equals("rights"))
					{
						try
						{
							rights = Long.parseLong(split[1]);
						}
						catch(NumberFormatException e)
						{
							rights = 0;
						}
					}
					// if regitem color was found
					else if(split[0].equals("color"))
					{
						// cut the " out of the second part of the line
						color = split[1].substring(1, split[1].length() - 1);
					}
					// if regitem lastlogin was found
					else if(split[0].equals("lastlogin"))
					{
						// cut the " out of the second part of the line
						lastlogin = split[1].substring(1, split[1].length() - 1);
					}
					else
					{
						// check if string
						if(split[1].startsWith("\"") && split[1].endsWith("\""))
						{
							// cut the " out of the second part of the line
							props.put(split[0], split[1].substring(1, split[1].length() - 1));
						}
						// if long
						else
						{
							try
							{
								props.put(split[0], new Long(split[1]));
							}
							catch(NumberFormatException e)
							{
							}
						}
					}
				}
			}

			// open file output
			FileOutputStream fileoutputstream = null;
			try
			{
				fileoutputstream = new FileOutputStream(m_dir + "/" + name.toLowerCase() + ".yrf");
			}
			catch(java.io.FileNotFoundException e)
			{
				System.out.println("Error opening " + name + ".yrf! Continue with next file...");
				continue;
			}

			PrintStream printstream = new PrintStream(fileoutputstream, true);

			printstream.println(name.toLowerCase());
			printstream.println(password);
			printstream.println("");
			printstream.println("---REGITEMS---");

			printstream.println("rights=(LONG)" + Long.toString(rights));
			printstream.println("color=(STRING)" + color);
			printstream.println("lastlogin=(STRING)" + lastlogin);


			for(Enumeration e = props.keys(); e.hasMoreElements();)
			{
				String key = (String)e.nextElement();
				Object value = props.get(key);
				String to_write = key + "=";
				if(value instanceof String)
					to_write += "(STRING)" +  (String)value;
				else if(value instanceof Long)
					to_write += "(LONG)" + ((Long)value).toString();
				else
					to_write += "(STRING)" + value.toString();

				printstream.println(to_write);
			}
		}

		return true;
	}

	private boolean deleteFolder(File file)
	{
		// check if file exists and is a directory
		if(!file.exists() || !file.isDirectory())
			return false;

		// get filelist
		String filelist[] = file.list();
		for(int i = 0; i < filelist.length; ++i)
		{
			// check if this file is a directory
			// if yes, then do a recursive call
			// if no, delete the file
			String s = file.getAbsolutePath() + "/" + filelist[i];
			File file1 = new File(s);
			if(file1.isDirectory())
				deleteFolder(file1);
			else
				file1.delete();
		}

		// delete the folder
		file.delete();

		return true;
	}

	private String readLine(BufferedReader br)
	{
		try
		{
			String to_ret = br.readLine();
			return to_ret;
		}
		catch(java.io.IOException e)
		{
			return null;
		}
	}
}
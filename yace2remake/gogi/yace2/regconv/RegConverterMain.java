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

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.FileInputStream;

public class RegConverterMain
{
	public static void main(String[] args)
	{
		// get registry folder
		System.out.println("YaCE 2 - Registry converter");

		File reg = null;

		while(true)
		{
			System.out.println();
			System.out.println("Please enter the folder where the registry files are stored:");

			// get the line into reg_folder
			String reg_folder = getLine();
			System.out.println();

			// check if the reg_folder exists and is a directory
			// and if permissions to read an write are set
			// otherwise repeat this step
			reg = new File(reg_folder);
			// check if reg_folder exists
			if(!reg.exists())
			{
				System.out.println("This folder does not exist!");
				continue;
			}

			// check if reg_folder is a directory
			if(!reg.isDirectory())
			{
				System.out.println(reg_folder + " is not a directory!");
				continue;
			}

			// check if read and write permissions are set
			if(!reg.canRead() || !reg.canWrite())
			{
				System.out.println("Permission denied: You have to set read and write permissions to this directory!");
				continue;
			}

			break;
		}

		System.out.println();

		// ask if automatically detect the file format
		System.out.println("The registry converter can automatically detect the file format.");
		System.out.println("Should the registry converter automatically detect the file format? (y/n)");

		// get the line into detect
		String detect = getLine();
		int file_format;
		System.out.println();

		// check if input is "y" or "n"
		if(detect.equals("y"))
		{
			System.out.println("Registry converter will now automatically detect the file format.");

			// auto detect
			file_format = autoDetect(reg);
			System.out.println(file_format);

			// check if a file was not readable
			switch(file_format)
			{
				case -1:
					System.out.println("At least one file could not be read!");
					System.out.println("Please check your permissions and start this program again.");
					System.exit(1);
					break;
				case 0:
					System.out.println("The file format could not be detected definitely.");
					System.out.println("Please enter the file format manually.");
					file_format = getFileFormat();
					break;
				case 1:
					System.out.println("The YaCE 1 file format was detected! Is this right? (y/n)");
					if(!getLine().equals("y"))
					{
						file_format = getFileFormat();
					}
					break;
				case 2:
					System.out.println("The YaCE 2 file format was detected! Is this right? (y/n)");
					if(!getLine().equals("y"))
					{
					file_format = getFileFormat();
					}
					break;
				case 3:
					System.out.println("The YaCE 2 Remake file format was detected! Is this right? (y/n)");
					if(!getLine().equals("y"))
					{
						file_format = getFileFormat();
					}
					break;
				default: // never happens
					break;
			}
		}
		else
		{
			file_format = getFileFormat();
		}

		System.out.println(file_format);

		// initialize the converter
		RegConverter conv = new RegConverter(reg.getPath(), file_format);
		System.out.println("Start converting files...");
		if(conv.convert())
		{
			System.out.println("Converting finished!");
			System.out.println("The old files have been stored in " + reg.getPath() + "old!");
			System.exit(0);
			// successful
		}
		else
		{
			// failed
			System.out.println("Converting failed! Check the output for more information.");
			System.exit(1);
		}
	}

	private static String getLine()
	{
		// read in a new line from the commandline and return the string
		System.out.print("> ");
		try
		{
			InputStreamReader rin = new InputStreamReader(System.in);
			BufferedReader din = new BufferedReader(rin);

			String line = din.readLine();
			if(line == null)
				return "";

			return line;
		}
		catch(java.io.IOException e)
		{
			return "";
		}
	}

	private static int autoDetect(File dir)
	{
		// returns -1 if a file was not readable
		// returns 0  if no file format was found
		// returns 1  if YaCE 1 file format
		// returns 2  if YaCE 2 file format
		// returns 3  if YaCE 2 Remake file format
		// ("reg_xxx" items are changed to "reginfo_xxx"
		//  and Object format is given in YaCE 2 Remake)

		// format = 4 is used if it cannot be divided between YaCE 2 and YaCE 2 Remake file format

		// get directory list
		String reglist[] = dir.list();

		// only check the first 3 files (or all files if there are less than 3 files in the directory)
		int to_check = (reglist.length < 3) ? reglist.length : 3;

		// this is the format that is returned
		int format = 0;

		for(int i = 0; i < to_check; ++i)
		{
			String file = reglist[i];

			// check if file is readable
			if(!(new File(dir.getAbsolutePath() + "/" + file)).canRead())
			{
				format = -1;
				break;
			}

			// check if YaCE 1 file
			if(file.endsWith(".yri"))
			{
				// check if the other file formats are also YaCE 1
				// if not, set format to 0 and break
				if((format == 1) || (format == 0))
					format = 1;
				else
				{
					format = 0;
					break;
				}
				// check next file
				continue;
			}

			// check if YaCE 2/YaCE 2 Remake file
			if(file.endsWith(".yrf"))
			{
				// open file to check if YaCE 2 or YaCE 2 Remake file
				FileInputStream fileinputstream = null;
				try
				{
					fileinputstream = new FileInputStream(dir.getAbsolutePath() + "/" + file);
				}
				catch(java.io.FileNotFoundException e)
				{
					format = -1;
					break;
				}
				BufferedReader bufferedreader = new BufferedReader(new InputStreamReader(fileinputstream));
				try
				{
					// read three lines and throw them away (1.: name, 2.: password, 3.: empty line)
					// there are some "equals()" to make a NullPointerException
					bufferedreader.readLine().equals("");
					bufferedreader.readLine().equals("");
					bufferedreader.readLine().equals("");

					// check if 4. line is "---REGITEMS---"
					if(!bufferedreader.readLine().equals("---REGITEMS---"))
					{
						format = 0;
						break;
					}
				}
				catch(java.io.IOException e)
				{
					format = 0;
					break;
				}
				catch(NullPointerException e)
				{
					format = -1;
					break;
				}

				String line;
				// opening a new try-catch block because file end could now be arrived
				// and then the converter would identify the files as undetected
				try
				{
					// read the next line
					line = bufferedreader.readLine();
				}
				catch(java.io.IOException e)
				{
					// if it cannot be devided between YaCE 2 and YaCE 2 Remake file format
					format = 4;
					continue;
				}

				// check for a "="
				if(line.indexOf("=") == -1)
				{
					format = -1;
					break;
				}

				// cut the value out of line
				String value = line.substring(line.indexOf("=") + 1);

				// check if YaCE 2 Remake file ("(<object type>)" at value beginning)
				if(value.startsWith("(LONG)") || value.startsWith("(STRING)") || value.startsWith("(BOOLEAN)"))
				{
					// check if the other file formats are also YaCE 2 Remake
					// if not, set format to 0 and break
					if((format == 3) || (format == 4) || (format == 0))
						format = 3;
					else
					{
						format = 0;
						break;
					}
				}
				else
				{
					// check if the other file formats are also YaCE 2
					// if not, set format to 0 and break
					if((format == 2) || (format == 4) || (format == 0))
						format = 2;
					else
					{
						format = 0;
						break;
					}
				}

				// check next file
				continue;
			}

			// if neither YaCE 1 nor YaCE 2/YaCE 2 Remake file, set format to 0 and break
			format = 0;
			break;
		}

		// check if format is 4
		if(format == 4)
			return 0;

		return format;
	}

	private static int getFileFormat()
	{
		int to_ret;
		while((to_ret = chooseFileFormat()) == 0)
		{
			System.out.println();
			System.out.println("You made an illegal input. Please enter a right number.");
		}
		return to_ret;
	}

	private static int chooseFileFormat()
	{
		// print out the choices
		System.out.println("Please enter the file format manually.");
		System.out.println("Choices:");
		System.out.println("1 - YaCE 1 file format");
		System.out.println("2 - YaCE 2 file format");

		// get the line
		String got = getLine();

		int format = 0;
		try
		{
			// parse the line to an integer
			format = Integer.parseInt(got);
		}
		catch(NumberFormatException e)
		{
			return 0;
		}

		if((format != 1) && (format != 2))
			return 0;

		return format;
	}
}
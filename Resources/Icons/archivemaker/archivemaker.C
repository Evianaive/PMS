/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *	Joe Drew
 *	Side Effects Software Inc
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *	Canada   M5J 2M2
 *	416-504-9876
 *
 * NAME:	archivemaker.C
 *
 * COMMENTS:
 */

#include <FS/FS_IndexFile.h>
#include <UT/UT_Array.h>
#include <UT/UT_DeepString.h>
#include <UT/UT_Defines.h>
#include <UT/UT_DirUtil.h>
#include <UT/UT_FileStat.h>
#include <UT/UT_OFStream.h>
#include <UT/UT_String.h>
#include <UT/UT_SysClone.h>
#include <UT/UT_SysSpecific.h>
#include <UT/UT_Tuple.h>
#include <UT/UT_WorkBuffer.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>

int
cmpname(UT_String *const*v1, UT_String *const*v2)
{
    if (!v1 && !v2)
	return 0;
    if (!v1)
	return -1;
    if (!v2)
	return 1;

    return strcmp((const char *)*(*v1),
		  (const char *)*(*v2));
}

typedef UT_Tuple<UT_DeepString, UT_DeepString, UT_FileStat> iconInfo;

void
addSubDirIcons(const char *dir, UT_Array<iconInfo> &iconlist)
{
    DIR			*dirp;
    struct dirent	*dp;
    char		 prevdir[BUFSIZ];


    if (!getcwd(prevdir, BUFSIZ))
    {
	fprintf(stderr, "Unable to getcwd\n");
	return;
    }

    if(chdir(dir) < 0)
    {
	fprintf(stderr, "Unable to chdir to \"%s\"\n", dir);
	return;
    }

    dirp = opendir(".");
    if (!dirp)
    {
	fprintf(stderr, "Unable to read contents of \"%s\"\n", dir);
	return;
    }

    while((dp = readdir(dirp)))
    {
	UT_ASSERT(errno != EOVERFLOW);

	UT_String 	 name;
	UT_FileStat	 stats;

	name = (const char *)dp->d_name;
	if (name == "." || name == "..")
	    continue;

	if (statLastRead(dirp, name, stats) != 0)
	{
	    fprintf(stderr, "unable to stat: \"%s\"\n", name.buffer());
	    continue;
	}

	if (stats.myFileType == UT_FileStat::DIRECTORY)
	    ; // ignore it
	else
	{
	    UT_String fullname;

	    if (!name.isstring())
		continue;

	    const char *ext = name.fileExtension();
	    if (!ext || (strcmp(".svg", name.fileExtension()) != 0))
		continue;

	    fullname = dir;
	    fullname += "/";
	    fullname += name;
	    UT_String filename(UT_String::ALWAYS_DEEP, fullname);

	    // Special case handlign for old .icon files converted to SVG.
	    if (strcmp(dir, "CONVERTED") == 0)
	    {
		fullname = name;
	    }
	    else
	    {
		fullname = dir;
		fullname.toUpper();
		fullname += "_";
		fullname += name;
	    }

	    UT_String sectionname(UT_String::ALWAYS_DEEP, fullname);

	    iconlist.append(iconInfo(filename, sectionname, stats));
	}
    }
    closedir (dirp);

    if(chdir(prevdir) < 0)
    {
	fprintf(stderr, "Unable to chdir back to \"%s\"\n", prevdir);
	return;
    }
}

int
main(int argc, char *argv[])
{
    if (argc > 3)
    {
	fprintf(stdout, "usage: %s [icondir] [destdir]\n", argv[0]);
	return 0;
    }

    const char *icondir, *destdir;
    if (argc == 2)
	destdir = icondir = argv[1];
    else if (argc == 3)
    {
        icondir = argv[1];
        destdir = argv[2];
    }
    else
	destdir = icondir = ".";

    UT_WorkBuffer iconindex;
    iconindex.sprintf("%s/SVGIcons.index", destdir);

    FS_IndexFile	 index;
    index.setDescription("Houdini SVG Icon Files");
    if(chdir(icondir) < 0)
    {
	fprintf(stderr, "Unable to chdir to \"%s\"\n", icondir);
	return -1;
    }

    DIR *dirp = opendir(".");
    if (!dirp)
    {
	fprintf(stderr, "Unable to read contents of \"%s\"\n", icondir);
	return - 1;
    }

    UT_Array<iconInfo> iconlist;
    UT_ValArray<UT_String *> dirlist;
    struct dirent *dp;
    while((dp = readdir(dirp)))
    {
	UT_ASSERT(errno != EOVERFLOW);

	UT_String	 name;
	UT_FileStat	 stats;

	name = (const char *)dp->d_name;
	if (name == "." || name == "..")
	    continue;

	if (statLastRead(dirp, name, stats) != 0)
	{
	    fprintf(stderr, "unable to stat: \"%s\"\n", name.buffer());
	    continue;
	}

	if (stats.myFileType == UT_FileStat::DIRECTORY)
	    dirlist.append(new UT_String(name, 1));
	else
	{
	    if (!name.isstring())
		continue;

	    const char *ext = name.fileExtension();
	    if (!ext || (strcmp(".svg", name.fileExtension()) != 0))
		continue;

	    UT_String filename(UT_String::ALWAYS_DEEP, name);
	    UT_String sectionname(UT_String::ALWAYS_DEEP, name);
	    iconlist.append(iconInfo(filename, sectionname, stats));
	}
    }
    closedir (dirp);

    int n = dirlist.entries();
    for (int i=0; i<n; i++)
	addSubDirIcons(*dirlist(i), iconlist);

    for (int i = 0; i < iconlist.entries(); ++i)
    {
	const char *name = UTtupleGet<0>(iconlist(i));
	const char *section_name = UTtupleGet<1>(iconlist(i));
	struct stat stats;

	if (0 != stat(name, &stats))
	{
	    fprintf(stderr, "Couldn't stat: \"%s\"\n", name);
	    continue;
	}

	if (index.hasSection(section_name))
	    std::cout << "Adding duplicate icon: " << section_name << std::endl;
	index.addSection(section_name, name);
	chmod(name, 0600);
    }

    UT_OFStream file(iconindex.buffer());
    index.writeFile(file);

    return 0;
}

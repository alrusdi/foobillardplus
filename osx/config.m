/*
 *  config.c
 *  foobilliard
 *
 *  Created by Julian Mayer on 27.03.08.
 *
 */

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>

char* get_mac_data_directory()
{
    putenv("SDL_ENABLEAPPEVENTS=1"); 

	char *buffer = calloc(1, 512);
	CFURLRef bundleurl = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFURLRef url = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, bundleurl, CFSTR("Contents/Resources/data/"), true);
	
	if (!CFURLGetFileSystemRepresentation(url, true, (UInt8*)buffer, 512))
	{
		exit(1);
	}
	else
		return buffer;
}

void open_file_with_browser_mac(char *file)
{
	NSString *app, *type;
	NSString *htmlFilePath = [[NSBundle mainBundle] pathForResource:@"index_a" ofType:@"html" inDirectory:@"data/locale/en/"];
	[[NSWorkspace sharedWorkspace] getInfoForFile:htmlFilePath application:&app type:&type];
	[[NSWorkspace sharedWorkspace] openFile:[NSString stringWithUTF8String:file] withApplication:app];
}
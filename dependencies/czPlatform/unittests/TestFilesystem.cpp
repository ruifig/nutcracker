#include "UnitTest++.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czUTF8String.h"
#include "czPlatform/czFilename.h"
#include "czPlatform/czFile.h"
#include "czPlatform/czString.h"

using namespace cz;

namespace unitests
{

	TEST(TestFilename)
	{
		Filename f("C:/ParentDir\\SubDir/Test.txt");
		CHECK(f.getFullPath() == "C:/ParentDir\\SubDir/Test.txt");
		auto dir = f.getDirectory();
		CHECK(dir=="C:/ParentDir\\SubDir/");
		auto fname = f.getFilename();
		CHECK(fname=="Test.txt");
		auto fbasename = f.getBaseFilename(); //
		CHECK(fbasename=="Test");
		auto extension1 = f.getExtension(false);
		auto extension2 = f.getExtension(true); //
		CHECK(extension1=="txt");
		CHECK(extension2==".txt");

		CHECK(Filename("C:/dir/file.txt").isRelativePath()==false);
		CHECK(Filename("/dir/file.txt").isRelativePath()==false);
		CHECK(Filename("dir/file.txt").isRelativePath()==true);
		CHECK(Filename("./dir/file.txt").isRelativePath()==true);
		CHECK(Filename("../dir/file.txt").isRelativePath()==true);
	}

	TEST(TestBinaryFile)
	{    
        // TODO Remove the path
		File f("Test.raw", File::FILEMODE_APPENDNEW);
		CHECK(f.isOpen()==true);
		
		int a[8] = {1,2,3,4,5,6,7,8};
		int b[4] = {0,0,0,0};
		CHECK(f.write(a, sizeof(a[0]), 8)==8);
		CHECK(f.tell()==sizeof(a));
		CHECK(f.seek(0, File::FILESEEK_SET));
		CHECK(f.read(b, sizeof(b[0]), 4)==4);
		CHECK_ARRAY_EQUAL(a, b, 4);
		CHECK(f.tell()==sizeof(b));
		CHECK(f.eof()==false);

		int c=0;
		CHECK(f.seek(-4*static_cast<int>(sizeof(int)), File::FILESEEK_CURRENT));
		CHECK(f.tell()==0);
		CHECK(f.read(&c, sizeof(c), 1)==1);
		CHECK(c==1);

		CHECK(f.seek(-4*static_cast<int>(sizeof(int)), File::FILESEEK_END));
		CHECK(f.tell()==4*sizeof(int));
		CHECK(f.read(&c, sizeof(c), 1)==1);
		CHECK(c==5);

		CHECK(f.seek(0, File::FILESEEK_END));
		CHECK(f.tell()==8*static_cast<int>(sizeof(int)));
		CHECK(f.eof()==false);
		
		/*
		CHECK(f.read(&c, sizeof(c), 1)==0);
		CHECK(f.eof());
		*/
	}
}

#include <iostream>
#include <azura.hpp>

using namespace std;
using namespace azura;


void RunBmpTests()
{
    /* Test read */

    cout << "Reading 'test.bmp'...";
    Image::Ptr image = ReadImage("../resources/test.bmp");
    if (!image) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;

    /* Test write */

    cout << "Writing 'out.bmp'...";
    bool succeeded = WriteImage(image, "out.bmp");
    if (!succeeded) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;
}

void RunPngTests()
{
    /* Test read */

    cout << "Reading 'test.png'...";
    Image::Ptr image = ReadImage("../resources/test.png");
    if (!image) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;

    /* Test write */

    cout << "Writing 'out.png'...";
    bool succeeded = WriteImage(image, "out.png");
    if (!succeeded) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;
}

void RunJpegTests()
{
    /* Test read */

    cout << "Reading 'test.jpg'...";
    Image::Ptr image = ReadImage("../resources/test.jpg");
    if (!image) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;

    /* Test write */

    cout << "Writing 'out.jpg'...";
    bool succeeded = WriteImage(image, "out.jpg");
    if (!succeeded) {
        cout << "failed" << endl;
        return;
    }
    cout << "done" << endl;
}

int main(int argc, char** argv)
{
    RunBmpTests();
    RunJpegTests();
    RunPngTests();

    return 0;
}

#ifndef _LEDSTRIP_H_
#define _LEDSTRIP_H_

struct PIXEL { byte r; byte g; byte b;};

class TLedStrip {

  private:
    int _num;
    struct PIXEL *_ledstrip;

  public:
    TLedStrip(int num);
    void init();
    void mirror(int posA, int posB, int length);
    void copy(int posA, int posB, int length);
    void shiftLeft(int pos, int length);
    void shiftRight(int pos, int length);
    void rotateRight(int pos, int length);
    void rotateLeft(int pos, int length);
    void fill(int pos, int length, byte *pArray);
    void blank();
    void white();
    
    void set(int pos, byte *pElement);
    void setRGB(int pos, byte r, byte g, byte b);
    void setHSV(int pos, int hue, byte  sat, byte val);

    int getCount() { return _num; };
    
    byte* get(int pos);
    
    void filter(int pos, int length, int coeff, byte *pRGB);
    void filter(byte *pTarget, byte *pRef, int coeff);
    byte filter(byte current, byte target, int coeff);

};

#endif

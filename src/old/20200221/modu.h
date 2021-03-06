#ifndef MODU_H
#define MODU_H

#include "resource.h"
#include "algvec.h"
#include "msq.h"

class Polygon {
public:
    uint8_t polytype;
    vector<uint8_t> bottomface;
    map<uint8_t, vector<uint8_t> > otherpoints;

    Polygon();
    ~Polygon();
};

struct moducache {
    uint8_t cnum, vnum, chosenC;
    vector<uint8_t> topo;
    map<uint8_t, Coord> coords;
    set<uint8_t> chosenV;
    map<uint8_t, uint8_t> vtkmapping;
};

class Modu {
public:
    uint8_t cnum, vnum, chosenC;
    vector<uint8_t> topo;
    map<uint8_t, Coord> coords;
    set<uint8_t> chosenV;
    int newcelltype;
    set<uint8_t> newcellpoints;
    set<set<uint8_t> > initedfaces;

    moducache cache;

    Modu();
    ~Modu();
    void ReadTopo(string filename);
    void AddNewcell(uint8_t);
    void VtkOut();
    void VtkOut(set<uint8_t>);
    void VtkIn();

    void SaveCache();
    void LoadCache();
    bool TryUntangle(float);
    void UntangleforNewCell(Polygon);

private:
    map<uint8_t, uint8_t> vtkmapping;
    void AddType0();
    void AddType1();
    void AddType2();
    void AddType3();

    void UntangleType1(set<set<uint8_t> >, float);
    void UntangleType2(set<set<uint8_t> >, float);
    void UntangleType3(set<set<uint8_t> >, float);
    void UntangleType4(set<set<uint8_t> >, float);
    void UntangleType4_1(set<set<uint8_t> >, float);
    void UntangleType4_2(set<set<uint8_t> >, float, Algvec);
    void UntangleType5(set<set<uint8_t> >, float);
    void Custom_push(vector<uint8_t>, vector<uint8_t>, set<uint8_t>, float);
    void PushOutwards(vector<uint8_t>, vector<uint8_t>, set<uint8_t>, Algvec, float);
    Coord movepos(Algvec, Coord, Coord, float);
    /* bool AddVirtualType1(set<set<uint8_t> > nfs); */
    /* bool AddVirtualType2(); */
    /* bool AddVirtualType3(); */

    /* void FindPosv1(vector<uint8_t>&, vector<uint8_t>&, vector<uint8_t>&, set<set<uint8_t> >); */
    Coord ProjecttoPlane(Coord, Coord, Algvec);
    Coord FinePoint(uint8_t, Polygon);
};
vector<uint8_t> fullcell(vector<uint8_t> oc);
#endif

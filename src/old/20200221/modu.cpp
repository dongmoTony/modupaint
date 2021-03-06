#include "modu.h"

Polygon::Polygon() {}
Polygon::~Polygon() {}

Modu::Modu() {
    cnum = 0;
    vnum = 0;
    chosenC = 0;
    topo = vector<uint8_t>();
    vtkmapping = map<uint8_t, uint8_t>();
    chosenV = set<uint8_t>();
    coords = map<uint8_t, Coord>();
    return;
}
Modu::~Modu(){}

void Modu::ReadTopo(string filename) {
    ifstream fin;
    fin.open(filename);
    uint32_t tmp;
    fin >> tmp;
    cnum = (uint8_t)tmp;
    topo.resize(cnum*8);
    for (uint8_t i=0; i<cnum; ++i) {
        for (uint8_t j=0; j<8; ++j) {
            fin >> tmp;
            topo[i*8+j] = (uint8_t)tmp;
        }
    }
    fin.close();
    set<uint8_t> tmpset;
    for (auto x : topo)
        tmpset.insert(x);
    vnum = tmpset.size();
    chosenC = 0;
    return;
}

void Modu::VtkOut() {
    //假定这里chosenV和chosenC已经就绪
    //vtkmapping也已经就绪
    ofstream fout;
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp.vtk";
    fout.open(filename);
    fout << "# vtk DataFile Version 3.0\n";
    fout << "Modu Geometry\n";
    fout << "ASCII\n";
    fout << "DATASET UNSTRUCTURED_GRID\n";
    fout << "POINTS " << chosenV.size() << " double\n";
    for (auto x : chosenV)
        fout << coords[x].x << " " << coords[x].y << " " << coords[x].z << endl;
    fout << "CELLS " << (int)chosenC << " " << (int)chosenC*9 << endl;
    for (int i=0; i<chosenC; ++i) {
        fout << "8 ";
        for (int j=0; j<8; ++j)
            fout << (int)topo[i*8+j] << " ";
        fout << endl;
    }
    fout << "CELL_TYPES " << (int)chosenC << endl;
    for (int i=0; i<chosenC; ++i)
        fout << "12" << endl;
    fout << "POINT_DATA " << (int)chosenV.size() << endl;
    fout << "SCALARS fixed int\nLOOKUP_TABLE default\n";
    for (int i=0; i<chosenV.size(); ++i) {
        if (i <= 1) fout << "1\n";
        else fout << "0\n";
    }
    fout.close();
}

void Modu::VtkOut(set<uint8_t> fixed) {
    ofstream fout;
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp.vtk";
    fout.open(filename);
    fout << "# vtk DataFile Version 3.0\n";
    fout << "Modu Geometry\n";
    fout << "ASCII\n";
    fout << "DATASET UNSTRUCTURED_GRID\n";
    fout << "POINTS " << chosenV.size() << " double\n";
    for (auto x : chosenV)
        fout << coords[x].x << " " << coords[x].y << " " << coords[x].z << endl;
    fout << "CELLS " << (int)chosenC << " " << (int)chosenC*9 << endl;
    for (int i=0; i<chosenC; ++i) {
        fout << "8 ";
        for (int j=0; j<8; ++j)
            fout << (int)topo[i*8+j] << " ";
        fout << endl;
    }
    fout << "CELL_TYPES " << (int)chosenC << endl;
    for (int i=0; i<chosenC; ++i)
        fout << "12" << endl;
    fout << "POINT_DATA " << (int)chosenV.size() << endl;
    fout << "SCALARS fixed int\nLOOKUP_TABLE default\n";
    for (int i=0; i<chosenV.size(); ++i) {
        //if (i <= 2) fout << "1\n";
        if (fixed.find(i) != fixed.end()) fout << "1\n";
        else fout << "0\n";
    }
    fout.close();
}

void Modu::VtkIn() {
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp_opt.vtk";
    ifstream fin;
    fin.open(filename);
    char buffer[256];
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    for (auto x : chosenV) {
        fin >> coords[x].x;
        fin >> coords[x].y;
        fin >> coords[x].z;
    }
    fin.close();
    return;
}

void Modu::AddNewcell(uint8_t c) {
    vector<uint8_t> cell(topo.begin()+c*8, topo.begin()+c*8+8);
    map<uint8_t, uint8_t> vpos;
    for (uint8_t i=0; i<8; ++i)
        vpos.insert(make_pair(cell[i], i));
    vector<uint8_t> fc = fullcell(cell);
    vector<uint8_t> faces;
    for (uint8_t i=0; i<6; ++i) {
        set<uint8_t> f(fc.begin()+i*4, fc.begin()+i*4+4);
        if (initedfaces.find(f) != initedfaces.end()) 
            faces.push_back(i);
    }
    if (faces.size() == 0) {
        coords.insert(make_pair(0, Coord{0, 0, 0}));
        coords.insert(make_pair(1, Coord{1, 0, 0}));
        coords.insert(make_pair(2, Coord{1, 1, 0}));
        coords.insert(make_pair(3, Coord{0, 1, 0}));
        coords.insert(make_pair(4, Coord{0, 0, 1}));
        coords.insert(make_pair(5, Coord{1, 0, 1}));
        coords.insert(make_pair(6, Coord{1, 1, 1}));
        coords.insert(make_pair(7, Coord{0, 1, 1}));
    }
    else if (faces.size() == 1) {
        Polygon poly;
        poly.polytype = 1;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        vector<uint8_t> bf = poly.bottomface;
        vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
        Coord cs[4] = {coords[bf[0]], coords[bf[1]], coords[bf[2]], coords[bf[3]]};
        for (uint8_t i=0; i<4; ++i) {
            Algvec v1(cs[(i+3)%4], cs[i]);
            Algvec v2(cs[i], cs[(i+1)%4]);
            Algvec vn = crossProduct(v1, v2);
            vn.Unit();
            Coord v3 = vn.getToPoint(cs[i]);
            coords[tf[i]] = v3;
        }
    }
    else if (faces.size() == 2) {
        Polygon poly;
        poly.polytype = 2;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        auto bf = poly.bottomface;
        for (uint8_t i=0; i<4; ++i) {//新的点定位置
            if (poly.otherpoints.find(tf[i])!=poly.otherpoints.end() or poly.otherpoints.find(tf[(i+1)%4])!=poly.otherpoints.end()) continue;
            Coord c1s[3] = {coords[bf[(i+3)%4]], coords[bf[i]], coords[tf[(i+3)%4]]}, 
                  c2s[3] = {coords[bf[(i+2)%4]], coords[bf[(i+1)%4]], coords[tf[(i+2)%4]]};
            Algvec n = crossProduct(Algvec(c1s[0], c1s[1]), Algvec(c1s[1], c2s[1]));
            Algvec n1 = crossProduct(Algvec(c1s[2], c1s[0]), Algvec(c1s[0], c2s[0]));
            n.Unit();
            n1.Unit();
            Algvec v1 = n + n1;
            v1.Unit();
            v1 = v1*1.414;
            if (dotProduct(v1, n) < 0) v1 = v1*(-1);
            coords[tf[i]] = v1.getToPoint(c1s[0]);
            /* Algvec v2 = Algvec(c2s[0], c2s[1]) + Algvec(c2s[0], c2s[2]); */
            /* if (v2.euLength() < 1e-3) v2 = n; */
            /* if (dotProduct(v2, n) < 0) v2 = v2*(-1); */
            /* v2.Unit(); */
            coords[tf[(i+1)%4]] = v1.getToPoint(c2s[0]);
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 3) {
        set<uint8_t> ftmp;
        Polygon poly;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 3;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        for (uint8_t i=0; i<3; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)==ftmp.end() or ftmp.find(f2)==ftmp.end()) continue;
            for (auto x : ftmp)
                if (x!=f1 and x!= f2) bfnum = x;
            poly.polytype = 4;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        if (poly.polytype == 3) {
            vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
            uint8_t np = 0;
            for (uint8_t i=0; i<4; ++i) {
                if (poly.otherpoints.find(tf[i]) == poly.otherpoints.end()) {
                    np = i;
                    break;
                }
            }
            vector<uint8_t> bf(poly.bottomface);
            uint8_t p0 = bf[(np+2)%4], p1 = tf[(np+2)%4], p2 = bf[(np+3)%4], p3 = bf[(np+1)%4];
            Coord cs[4] = {coords[p0], coords[p1], coords[p2], coords[p3]};
            Algvec v;
            v.dx = 0, v.dy = 0, v.dz = 0;
            for (uint8_t i=0; i<3; ++i) {
                Algvec v1(cs[0], cs[i+1]);
                Algvec v2(cs[0], cs[(i+1)%3+1]);
                v = v + crossProduct(v1, v2);
            }
            v.Unit();
            v = v*1.732;
            coords[tf[np]] = v.getToPoint(cs[0]);
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 4) {
        set<uint8_t> ftmp;
        Polygon poly;
        uint8_t bfnum;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 5;
        for (uint8_t i=0; i<4; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)==ftmp.end() or ftmp.find(f2)==ftmp.end()) continue;
            for (auto x : ftmp)
                if (x!=f1 and x!= f2) bfnum = x;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 5) {
        set<uint8_t> ftmp;
        Polygon poly;
        uint8_t bfnum;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 6;
        for (uint8_t i=0; i<5; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)!=ftmp.end() and ftmp.find(f2)!=ftmp.end()) continue;
            if (ftmp.find(f1) == ftmp.end()) bfnum = f1;
            else if (ftmp.find(f2) == ftmp.end()) bfnum = f2;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        UntangleforNewCell(poly);
    }
    set<uint8_t> f0 = {cell[0], cell[1], cell[2], cell[3]};
    set<uint8_t> f1 = {cell[0], cell[1], cell[5], cell[4]};
    set<uint8_t> f2 = {cell[1], cell[2], cell[6], cell[5]};
    set<uint8_t> f3 = {cell[2], cell[3], cell[7], cell[6]};
    set<uint8_t> f4 = {cell[3], cell[0], cell[4], cell[7]};
    set<uint8_t> f5 = {cell[4], cell[5], cell[6], cell[7]};
    initedfaces.insert(f0);
    initedfaces.insert(f1);
    initedfaces.insert(f2);
    initedfaces.insert(f3);
    initedfaces.insert(f4);
    initedfaces.insert(f5);
    chosenC += 1;
    for (auto x : cell)
        chosenV.insert(x);
    /* int newv = 0; */
    /* for (uint8_t i=0; i<8; ++i) { */
    /*     if (chosenV.find(topo[chosenC*8+i]) == chosenV.end()) { */
    /*         vtkmapping.insert(make_pair(topo[chosenC*8+i], chosenV.size())); */
    /*         chosenV.insert(topo[chosenC*8+i]); */
    /*         newv += 1; */
    /*     } */
    /* } */
    /* chosenC += 1; */
    /* if (newv == 8) { */
    /*     AddType0(); */
    /*     newcelltype = 0; */
    /* } */
    /* else if (newv == 4) { */
    /*     AddType1(); */
    /*     newcelltype = 1; */
    /* } */
    /* else if (newv == 2) { */
    /*     AddType2(); */
    /*     newcelltype = 2; */
    /* } */
    /* else if (newv == 1) { */
    /*     AddType3(); */
    /*     newcelltype = 3; */
    /* } */
    /* else */
    /*     newcelltype = 4; */
}

/* void Modu::AddType0() { */
/*     coords.insert(make_pair(0, Coord{0, 0, 0})); */
/*     coords.insert(make_pair(1, Coord{1, 0, 0})); */
/*     coords.insert(make_pair(2, Coord{1, 1, 0})); */
/*     coords.insert(make_pair(3, Coord{0, 1, 0})); */
/*     coords.insert(make_pair(4, Coord{0, 0, 1})); */
/*     coords.insert(make_pair(5, Coord{1, 0, 1})); */
/*     coords.insert(make_pair(6, Coord{1, 1, 1})); */
/*     coords.insert(make_pair(7, Coord{0, 1, 1})); */
/* } */

/* void Modu::AddType1() { */
/*     uint8_t c = chosenC - 1; */
/*     for (int k=0; k<4; k++) { */
/*         Coord c1 = coords[topo[c*8+k]]; */
/*         Coord c2 = coords[topo[c*8+(k+1)%4]]; */
/*         Coord c3 = coords[topo[c*8+(k+3)%4]]; */
/*         auto v1 = crossProduct(Algvec(c1, c2), Algvec(c1, c3)); */
/*         v1.Unit(); */
/*         coords.insert(make_pair(topo[c*8+k+4], v1.getToPoint(c1))); */
/*     } */
/* } */

/* void Modu::AddType2() { */
/*     vector<uint8_t> newpoints; */
/*     for (auto x : chosenV) { */
/*         if (coords.find(x) == coords.end()) */
/*             newpoints.push_back(x); */
/*     } */
/*     uint8_t c = chosenC - 1; */
/*     for (int i=0; i<2; ++i) { */
/*         int fp = 0, fn = 0; */
/*         auto p = newpoints[i], pn = newpoints[1-i]; */
/*         for (auto k=0; k<4; ++k) { */
/*             if (topo[c*8+k+4] == p) { */
/*                 fp = k; */
/*                 fn = 1; */
/*                 if (topo[c*8+(k+1)%4+4] == pn) */
/*                     fn = 3; */
/*                 break; */
/*             } */
/*         } */
/*         auto p1 = topo[c*8+4+(fp+fn)%4]; */
/*         auto p2 = topo[c*8+fp]; */
/*         auto p3 = topo[c*8+(fp+fn)%4]; */
/*         auto p4 = topo[c*8+(fp+2)%4]; */
/*         Algvec vn; */
/*         if (fn == 3) vn = Algvec(coords[p4], coords[p3]); */
/*         else if (fn == 1) vn = Algvec(coords[p3], coords[p4]); */
/*         auto v = Algvec(coords[p3], coords[p1]) + */ 
/*                  Algvec(coords[p3], coords[p2]); */
/*         auto vp = crossProduct(Algvec(coords[p3], coords[p2]), Algvec(coords[p3], coords[p1])); */
/*         if (v.euLength() < 1e-2) */
/*             v = crossProduct(vn, Algvec(coords[p3], coords[p2]))+Algvec(coords[p3], coords[p2]); */
/*         v.Unit(); */
/*         v = v*sqrt(2); */
/*         if (dotProduct(vp, vn) < 0) */
/*             v = -1*v; */
/*         coords[p] = v.getToPoint(coords[p3]); */
/*     } */
/* } */

/* void Modu::AddType3() { */
/*     uint8_t c = chosenC - 1; */
/*     uint8_t newpoint; */
/*     for (auto x : chosenV) { */
/*         if (coords.find(x) == coords.end()) */
/*             newpoint = x; */
/*     } */
/*     vector<uint8_t> p(4); */
/*     for (int i=0; i<4; ++i) { */
/*         if (topo[c*8+4+i] == newpoint) { */
/*             p[0] = topo[c*8+(i+1)%4]; */
/*             p[1] = topo[c*8+(i+2)%4+4]; */
/*             p[2] = topo[c*8+(i+3)%4]; */
/*             p[3] = topo[c*8+(i+2)%4]; */
/*             break; */
/*         } */
/*     } */
/*     Coord coo[3]; */
/*     for (int i=0; i<3; ++i) { */
/*         auto v = crossProduct(Algvec(coords[p[3]], coords[p[i]]), */  
/*                  Algvec(coords[p[3]], coords[p[(i+1)%3]])); */
/*         v.Unit(); */
/*         coo[i] = v.getToPoint(coords[p[3]]); */
/*     } */
/*     Coord nc{0, 0, 0}; */
/*     for (int i=0; i<3; ++i) { */
/*         nc.x += coo[i].x/3; */
/*         nc.y += coo[i].y/3; */
/*         nc.z += coo[i].z/3; */
/*     } */
/*     coords.insert(make_pair(newpoint, nc)); */
/* } */

/* void Modu::SaveCache() { */
/*     cache.cnum = cnum; */
/*     cache.vnum = vnum; */
/*     cache.chosenC = chosenC; */
/*     cache.chosenV = chosenV; */
/*     cache.coords = coords; */
/*     cache.topo = topo; */
/*     cache.vtkmapping = vtkmapping; */
/* } */

/* void Modu::LoadCache() { */
/*     cnum = cache.cnum; */
/*     vnum = cache.vnum; */
/*     chosenC = cache.chosenC; */
/*     chosenV = cache.chosenV; */
/*     coords = cache.coords; */
/*     topo = cache.topo; */
/*     vtkmapping = cache.vtkmapping; */
/* } */

/* bool Modu::TryUntangle(float para) { */
/*     //判断类型 */
/*     set<set<uint8_t> > fs; */
/*     for (auto i=0; i<chosenC; ++i) { */
/*         set<uint8_t> faces[6]; */
/*         for (int j=0; j<4; ++j) { */
/*             faces[0].insert(topo[i*8+j]); */
/*             faces[5].insert(topo[i*8+j+4]); */
/*         } */
/*         for (int j=0; j<4; ++j) { */
/*             faces[j+1].insert(topo[i*8+j]); */
/*             faces[j+1].insert(topo[i*8+(j+1)%4]); */
/*             faces[j+1].insert(topo[i*8+j+4]); */
/*             faces[j+1].insert(topo[i*8+(j+1)%4+4]); */
/*         } */
/*         for (int j=0; j<6; ++j) */
/*             fs.insert(faces[j]); */
/*     } */
/*     set<uint8_t> newc_faces[6]; */
/*     for (int j=0; j<4; ++j) { */
/*         newc_faces[0].insert(topo[chosenC*8+j]); */
/*         newc_faces[5].insert(topo[chosenC*8+j+4]); */
/*     } */
/*     for (int j=0; j<4; ++j) { */
/*         newc_faces[j+1].insert(topo[chosenC*8+j]); */
/*         newc_faces[j+1].insert(topo[chosenC*8+(j+1)%4]); */
/*         newc_faces[j+1].insert(topo[chosenC*8+j+4]); */
/*         newc_faces[j+1].insert(topo[chosenC*8+(j+1)%4+4]); */
/*     } */
/*     int oldfnum = 0; */
/*     set<set<uint8_t> > nfs; */
/*     for (auto f : newc_faces) { */
/*         if (fs.find(f) != fs.end()) { */
/*             oldfnum += 1; */
/*             nfs.insert(f); */
/*         } */
/*     } */
/*     newcellpoints.clear(); */
/*     for (auto f : nfs) */
/*         for (auto x : f) */
/*             newcellpoints.insert(x); */
/*     if (oldfnum == 3) { */
/*         map<uint8_t, uint8_t> valence; */
/*         for (auto f : nfs) */
/*             for (auto x : f) { */
/*                 if (valence.find(x) == valence.end()) */
/*                     valence.insert(make_pair(x, 0)); */
/*                 valence[x] += 1; */
/*             } */
/*         int type = 1; */
/*         for (auto x : valence) */
/*             if (x.second == 3) */
/*                 type = 5; */
/*         if (type == 1) */
/*             UntangleType1(nfs, para); */
/*         else */
/*             UntangleType5(nfs, para); */
/*     } */
/*     else if (oldfnum == 4) UntangleType2(nfs, para); */
/*     else if (oldfnum == 5) UntangleType3(nfs, para); */
/*     else if (oldfnum == 2) UntangleType4(nfs, para); */
/*     return true; */
/* } */

/* void Modu::UntangleType1(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> bottomface; */
/*     set<uint8_t> others; */
    
/*     for (auto f : nfs) { */
/*         for (auto x : f) { */
/*             if (others.find(x) != others.end()) { */
/*                 others.erase(others.find(x)); */
/*                 bottomface.insert(x); */
/*             } */
/*             else */
/*                 others.insert(x); */
/*         } */
/*     } */
/*     vector<uint8_t> tmp; */
/*     tmp.insert(tmp.end(), topo.begin()+chosenC*8, topo.begin()+chosenC*8+8); */
/*     set<uint8_t> bottompos; */
/*     for (int i=0; i<8; ++i) { */
/*         if (bottomface.find(tmp[i]) != bottomface.end()) */
/*             bottompos.insert(i); */
/*     } */
/*     vector<uint8_t> b1, b2; */
/*     if (bottompos == set<uint8_t>{0, 1, 2, 3}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[2]}; */
/*         b2 = vector<uint8_t>{tmp[2], tmp[3], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 1, 4, 5}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[5]}; */
/*         b2 = vector<uint8_t>{tmp[5], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{1, 2, 5, 6}) { */
/*         b1 = vector<uint8_t>{tmp[1], tmp[2], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[5], tmp[1]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{2, 3, 6, 7}) { */
/*         b1 = vector<uint8_t>{tmp[2], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[6], tmp[2]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 3, 4, 7}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{4, 5, 6 ,7}) { */
/*         b1 = vector<uint8_t>{tmp[4], tmp[5], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[7], tmp[4]}; */
/*     } */
/*     Custom_push(b1, b2, others, para); */
/* } */
/* void Modu::UntangleType2(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> bottomface; */
/*     set<uint8_t> others; */
/*     map<uint8_t, int> allpoints; */
/*     for (auto f : nfs) { */
/*         for (auto x : f) { */
/*             if (allpoints.find(x) == allpoints.end()) */
/*                 allpoints.insert(make_pair(x, 1)); */
/*             else */
/*                 allpoints[x]++; */
/*         } */
/*     } */
/*     for (auto f : nfs) { */
/*         int qq = 1; */
/*         for (auto x : f) { */
/*             if (allpoints[x] == 1) { */
/*                 qq = 0; */
/*                 break; */
/*             } */
/*         } */
/*         if (qq == 1) { */
/*             bottomface = f; */
/*             for (auto x : allpoints) { */
/*                 if (f.find(x.first) == f.end()) */
/*                     others.insert(x.first); */
/*             } */
/*             break; */
/*         } */
/*     } */
/*     vector<uint8_t> tmp; */
/*     tmp.insert(tmp.end(), topo.begin()+chosenC*8, topo.begin()+chosenC*8+8); */
/*     set<uint8_t> bottompos; */
/*     for (int i=0; i<8; ++i) { */
/*         if (bottomface.find(tmp[i]) != bottomface.end()) */
/*             bottompos.insert(i); */
/*     } */
/*     vector<uint8_t> b1, b2; */
/*      if (bottompos == set<uint8_t>{0, 1, 2, 3}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[2]}; */
/*         b2 = vector<uint8_t>{tmp[2], tmp[3], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 1, 4, 5}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[5]}; */
/*         b2 = vector<uint8_t>{tmp[5], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{1, 2, 5, 6}) { */
/*         b1 = vector<uint8_t>{tmp[1], tmp[2], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[5], tmp[1]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{2, 3, 6, 7}) { */
/*         b1 = vector<uint8_t>{tmp[2], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[6], tmp[2]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 3, 4, 7}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{4, 5, 6 ,7}) { */
/*         b1 = vector<uint8_t>{tmp[4], tmp[5], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[7], tmp[4]}; */
/*     } */
/*     Custom_push(b1, b2, others, para); */

/* } */

/* void Modu::UntangleType3(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> bottomface; */
/*     set<uint8_t> others; */
/*     map<uint8_t, int> allpoints; */
/*     for (auto f : nfs) { */
/*         for (auto x : f) { */
/*             if (allpoints.find(x) == allpoints.end()) */
/*                 allpoints.insert(make_pair(x, 1)); */
/*             else */
/*                 allpoints[x]++; */
/*         } */
/*     } */
/*     int qq = 1; */
/*     for (auto f : nfs) { */
/*         for (auto x : f) { */
/*             if (allpoints[x] == 2) { */
/*                 qq = 0; */
/*                 continue; */
/*             } */
/*         } */
/*         if (qq == 1) { */
/*             bottomface = f; */
/*             for (auto x : allpoints) { */
/*                 if (f.find(x.first) == f.end()) */
/*                     others.insert(x.first); */
/*             } */
/*             break; */
/*         } */
/*     } */
/*     vector<uint8_t> tmp; */
/*     tmp.insert(tmp.end(), topo.begin()+chosenC*8, topo.begin()+chosenC*8+8); */
/*     set<uint8_t> bottompos; */
/*     for (int i=0; i<8; ++i) { */
/*         if (bottomface.find(tmp[i]) != bottomface.end()) */
/*             bottompos.insert(i); */
/*     } */
/*     vector<uint8_t> b1, b2; */
/*      if (bottompos == set<uint8_t>{0, 1, 2, 3}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[2]}; */
/*         b2 = vector<uint8_t>{tmp[2], tmp[3], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 1, 4, 5}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[1], tmp[5]}; */
/*         b2 = vector<uint8_t>{tmp[5], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{1, 2, 5, 6}) { */
/*         b1 = vector<uint8_t>{tmp[1], tmp[2], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[5], tmp[1]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{2, 3, 6, 7}) { */
/*         b1 = vector<uint8_t>{tmp[2], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[6], tmp[2]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{0, 3, 4, 7}) { */
/*         b1 = vector<uint8_t>{tmp[0], tmp[3], tmp[7]}; */
/*         b2 = vector<uint8_t>{tmp[7], tmp[4], tmp[0]}; */
/*     } */
/*     else if (bottompos == set<uint8_t>{4, 5, 6 ,7}) { */
/*         b1 = vector<uint8_t>{tmp[4], tmp[5], tmp[6]}; */
/*         b2 = vector<uint8_t>{tmp[6], tmp[7], tmp[4]}; */
/*     } */
/*     Custom_push(b1, b2, others, para); */

/* } */

/* void Modu::UntangleType4(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> facepos[2]; */
/*     Algvec n[2]; */
/*     vector<uint8_t> newc(topo.begin()+chosenC*8, topo.begin()+chosenC*8+8); */
/*     int i=0; */
/*     for (auto it=nfs.begin(); i<2; ++i, ++it) { */
/*         auto face = *it; */
/*         for (auto x : face) { */
/*             for (int j=0; j<8; ++j) { */
/*                 if (x == newc[j]) */
/*                     facepos[i].insert(j); */
/*             } */
/*         } */
/*         Coord p1, p2, p3, p4; */
/*         if (facepos[i] == set<uint8_t>{0, 1, 2, 3}) */
/*             p1 = coords[newc[0]], p2 = coords[newc[3]], p3 = coords[newc[2]], p4 = coords[newc[1]]; */
/*         else if (facepos[i] == set<uint8_t>{0, 1, 4, 5}) */
/*             p1 = coords[newc[0]], p2 = coords[newc[1]], p3 = coords[newc[5]], p4 = coords[newc[4]]; */
/*         else if (facepos[i] == set<uint8_t>{1, 2, 5, 6}) */
/*             p1 = coords[newc[1]], p2 = coords[newc[2]], p3 = coords[newc[6]], p4 = coords[newc[5]]; */
/*         else if (facepos[i] == set<uint8_t>{2, 3, 6, 7}) */
/*             p1 = coords[newc[2]], p2 = coords[newc[3]], p3 = coords[newc[7]], p4 = coords[newc[6]]; */
/*         else if (facepos[i] == set<uint8_t>{0, 3, 4, 7}) */
/*             p1 = coords[newc[3]], p2 = coords[newc[0]], p3 = coords[newc[4]], p4 = coords[newc[7]]; */
/*         else if (facepos[i] == set<uint8_t>{4, 5, 6, 7}) */
/*             p1 = coords[newc[4]], p2 = coords[newc[5]], p3 = coords[newc[6]], p4 = coords[newc[7]]; */
/*         auto n1 = crossProduct(Algvec(p1, p2), Algvec(p2, p3)); n1.Unit(); */
/*         auto n2 = crossProduct(Algvec(p2, p3), Algvec(p3, p4)); n2.Unit(); */
/*         auto n3 = crossProduct(Algvec(p3, p4), Algvec(p4, p1)); n3.Unit(); */
/*         auto n4 = crossProduct(Algvec(p4, p1), Algvec(p1, p2)); n4.Unit(); */
/*         n[i] = (n1+n2+n3+n4); */
/*     } */
/*     if (dotProduct(n[0], n[1])/(n[0].euLength()*n[1].euLength()) < -0.8) */
/*         UntangleType4_2(nfs, para, -1*n[0]); */
/*     else */
/*         UntangleType4_1(nfs, para); */
/* } */

/* void Modu::UntangleType4_1(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> bottomface, others, allp; */
/*     for (auto f : nfs) */
/*         for (auto x : f) { */
/*             if (allp.find(x) != allp.end()) */
/*                 others.insert(x); */
/*             else */
/*                 allp.insert(x); */
/*         } */
/*     for (auto x : allp) */
/*         if (others.find(x) != others.end()) */
/*             bottomface.insert(x); */
/*     vector<uint8_t> bot(bottomface.begin(), bottomface.end()); */
/*     vector<uint8_t> b1{bot[0], bot[1], bot[2]}, b2{bot[0], bot[1], bot[3]}; */
/*     Custom_push(b1, b2, others, para); */
/* } */

/* void Modu::UntangleType4_2(set<set<uint8_t> > nfs, float para, Algvec n) { */
/*     set<uint8_t> others; */
/*     set<uint8_t> bottomface = *nfs.begin(); */
/*     for (auto x : *nfs.rbegin()) */
/*         if (bottomface.find(x) == bottomface.end()) */
/*             others.insert(x); */
/*     vector<uint8_t> bot(bottomface.begin(), bottomface.end()); */
/*     vector<uint8_t> b1{bot[0], bot[1], bot[2]}, b2{bot[0], bot[1], bot[3]}; */
/*     PushOutwards(b1, b2, others, n, para); */
/* } */

/* void Modu::UntangleType5(set<set<uint8_t> > nfs, float para) { */
/*     set<uint8_t> others; */
/*     vector<uint8_t> b; */
/*     map<uint8_t, uint8_t> valence; */
/*     for (auto f : nfs) { */
/*         for (auto x : f) { */
/*             if (valence.find(x) == valence.end()) */
/*                 valence.insert(make_pair(x, 0)); */
/*             valence[x] += 1; */
/*         } */
/*     } */
/*     for (auto x : valence) { */
/*         if (x.second == 2) */
/*             b.push_back(x.first); */
/*         else */
/*             others.insert(x.first); */
/*     } */
/*     Custom_push(b, b, others, para); */
/* } */

/* void Modu::Custom_push(vector<uint8_t> b1, vector<uint8_t> b2, set<uint8_t> others, float para) { */
/*     Algvec n1 = crossProduct(Algvec(coords[b1[0]], coords[b1[1]]), Algvec(coords[b1[1]], coords[b1[2]])); */
/*     Algvec n2 = crossProduct(Algvec(coords[b2[0]], coords[b2[1]]), Algvec(coords[b2[1]], coords[b2[2]])); */
/*     n1.Unit(); */
/*     n2.Unit(); */
/*     auto n = n1 + n2; */
/*     for (auto p : others) coords[p] = movepos(n, coords[b1[1]], coords[p], para); */
/* } */

void Modu::PushOutwards(vector<uint8_t> b1, vector<uint8_t> b2, set<uint8_t> others, Algvec n, float para) {
    for (auto p : others)
        coords[p] = {coords[p].x+n.dx*para, coords[p].y+n.dy*para, coords[p].z+n.dz*para};
}

Coord Modu::movepos(Algvec n, Coord b, Coord p, float para) {
    double l;
    double x0, y0, z0, x1, y1, z1, i, j, k;
    x0=b.x, y0=b.y, z0=b.z, x1=p.x, y1=p.y, z1=p.z, i=n.dx, j=n.dy, k=n.dz;
    l = i*x0+j*y0+k*z0-i*x1-j*y1-k*z1;
    l = l/(i*i+j*j+k*k);
    Coord res{x1+l*i*para, y1+l*j*para, z1+l*k*para};
    return res;
}

vector<uint8_t> fullcell(vector<uint8_t> oc) {
    vector<uint8_t> res;
    res.push_back(oc[0]);
    res.push_back(oc[3]);
    res.push_back(oc[2]);
    res.push_back(oc[1]);
    res.push_back(oc[0]);
    res.push_back(oc[1]);
    res.push_back(oc[5]);
    res.push_back(oc[4]);
    res.push_back(oc[1]);
    res.push_back(oc[2]);
    res.push_back(oc[6]);
    res.push_back(oc[5]);
    res.push_back(oc[2]);
    res.push_back(oc[3]);
    res.push_back(oc[7]);
    res.push_back(oc[6]);
    res.push_back(oc[3]);
    res.push_back(oc[0]);
    res.push_back(oc[4]);
    res.push_back(oc[7]);
    res.push_back(oc[4]);
    res.push_back(oc[5]);
    res.push_back(oc[6]);
    res.push_back(oc[7]);
    //另一组，第一组的对面
    res.push_back(oc[4]);
    res.push_back(oc[7]);
    res.push_back(oc[6]);
    res.push_back(oc[5]);
    res.push_back(oc[3]);
    res.push_back(oc[2]);
    res.push_back(oc[6]);
    res.push_back(oc[7]);
    res.push_back(oc[0]);
    res.push_back(oc[3]);
    res.push_back(oc[7]);
    res.push_back(oc[4]);
    res.push_back(oc[1]);
    res.push_back(oc[0]);
    res.push_back(oc[4]);
    res.push_back(oc[5]);
    res.push_back(oc[2]);
    res.push_back(oc[1]);
    res.push_back(oc[5]);
    res.push_back(oc[6]);
    res.push_back(oc[0]);
    res.push_back(oc[1]);
    res.push_back(oc[2]);
    res.push_back(oc[3]);
    return res;
}

void Modu::UntangleforNewCell(Polygon poly) {
    Mesquite::MsqError err;
    Algvec n1, n2, n;
    Coord bfc[4] = {coords[poly.bottomface[0]], coords[poly.bottomface[1]], 
                    coords[poly.bottomface[2]], coords[poly.bottomface[3]]};
    n1 = crossProduct(Algvec(bfc[0], bfc[1]), Algvec(bfc[1], bfc[2]));
    n2 = crossProduct(Algvec(bfc[2], bfc[3]), Algvec(bfc[3], bfc[0]));
    n = n1 + n2;
    n.Unit();
    for (auto x : poly.otherpoints) {
        Coord p = coords[x.first];
        if (dotProduct(n, Algvec(p, bfc[0])) < 0) continue;
        //Coord target1 = ProjecttoPlane(p, bfc[0], n);
        Coord target2 = FinePoint(x.first, poly);
        Algvec ln = n*0.1;
        Coord target3 = ln.getToPoint(target2);
        //Coord base = coords[x.second[0]];
        //if (dotProduct(Algvec(base, target2), Algvec(target1, target2)) < 0)
            //coords[x.first] = target1;
        //else
            //coords[x.first] = target2;
        coords[x.first] = target3;
    }
}

Coord Modu::ProjecttoPlane(Coord s, Coord b, Algvec n) {
    double para = 1.1;
    double x0, y0, z0, x1, y1, z1, i, j, k;
    x0=b.x, y0=b.y, z0=b.z;
    x1=s.x, y1=s.y, z1=s.z, i=n.dx, j=n.dy, k=n.dz;
    auto l = i*x0+j*y0+k*z0-i*x1-j*y1-k*z1;
    l = l/(i*i+j*j+k*k);
    Coord res{x1+l*i*para, y1+l*j*para, z1+l*k*para};
    return res;
}

Coord Modu::FinePoint(uint8_t p, Polygon poly) {
    Coord base = coords[poly.otherpoints[p][0]];
    Coord p1 = coords[poly.otherpoints[p][1]];
    Coord p2 = base;
    if (poly.otherpoints[p].size() == 3)
        p2 = coords[poly.otherpoints[p][2]];
    Algvec v = Algvec(p1, base) + Algvec(p2, base);
    v.Unit();
    Algvec u(base, p1);
    v = v*u.euLength();
    return v.getToPoint(base);
}


/* bool Modu::AddVirtualType1(set<set<uint8_t> > nfs) { */
/*     vector<uint8_t> bottomface, topface; */
/*     vector<uint8_t> nbfacepos; */

/*     FindPosv1(bottomface, topface, nbfacepos, nfs); */

/*     int untanglefacenum = 2; */
/*     for (int i=0; i<untanglefacenum; ++i) { */
/*         Modu tempmodu;//以cache为标志 */
/*         tempmodu.chosenC = cache.chosenC; */
/*         tempmodu.coords = cache.coords; */
/*         tempmodu.chosenV = cache.chosenV; */
/*         tempmodu.topo.insert(tempmodu.topo.end(), cache.topo.begin(), cache.topo.begin()+cache.chosenC*8);//取前chosenC个 */
/*         vector<uint8_t> newcell; */
/*         newcell.insert(newcell.end(), bottomface.begin(), bottomface.end()); */
/*         vector<uint8_t> t = topface; */
/*         int vn = tempmodu.chosenV.size(); */
/*         t[nbfacepos[i*2]] = vn++; */
/*         t[nbfacepos[i*2+1]] = vn++; */
/*         newcell.insert(newcell.end(), t.begin(), t.end()); */
/*         tempmodu.AddNewcell(); */
/*         tempmodu.VtkOut(); */
/*         //优化 */
/*         if (!Untangle(tempmodu.chosenC)) */
/*             return false; */
/*         //优化结束 */
/*         tempmodu.VtkIn(); */
/*         for (int j=0; j<tempmodu.chosenV.size()-1; ++j) { */
/*             coords[j] = tempmodu.coords[j]; */
/*         } */
/*         for (int j=0; j<8; ++j) { */
/*             if (j!=nbfacepos[i*2] and j!=nbfacepos[i*2+1]) { */
/*                 coords[topo[(tempmodu.chosenC-1)*8+j]] = tempmodu.coords[tempmodu.topo[(tempmodu.chosenC-1)*8+j]]; */
/*             } */
/*         } */
/*     } */
/*     return true; */
/* } */

/* void Modu::FindPosv1(vector<uint8_t> &bottomface, vector<uint8_t> &topface, vector<uint8_t> &nbfacepos, set<set<uint8_t> > nfs) { */
/*     set<uint8_t> bf, tf, of[2]; */
/*     for (auto f : nfs) { */
/*         int nbhf = 0; */
/*         for (auto f1 : nfs) { */
/*             if (f == f1) continue; */
/*             int cp = 0; */
/*             for (auto x : f) */
/*                 if (f1.find(x) != f1.end()) cp++; */
/*             if (cp == 2) nbhf++; */
/*         } */
/*         if (nbhf == 2) { */
/*             bf = f; */
/*             int ofn = 0; */
/*             for (auto xf : nfs) */
/*                 if (xf != f) of[ofn++] = xf; */
/*             break; */
/*         } */
/*     } */
/*     vector<uint8_t> nc; */
/*     nc.insert(nc.end(), topo.begin()+cache.chosenC*8, topo.begin()+cache.chosenC*8+8); */
/*     set<uint8_t> ncpos; */
/*     for (auto x : bf) { */
/*         for (uint8_t i=0; i<8; ++i) { */
/*             if (nc[i] == x) */
/*                 ncpos.insert(i); */
/*         } */
/*     } */
/*     if (ncpos == set<uint8_t>{0, 1, 2, 3}) { */
/*         bottomface.insert(bottomface.end(), nc.begin(), nc.begin()+4); */
/*         topface.insert(topface.end(), nc.begin()+4, nc.begin()+8); */
/*     } */
/*     else if (ncpos == set<uint8_t>{4, 5, 6, 7}) { */
/*         bottomface.insert(bottomface.end(), nc.begin()+4, nc.begin()+8); */
/*         topface.insert(topface.end(), nc.begin(), nc.begin()+4); */
/*         auto temp = bottomface[1]; */
/*         bottomface[1] = bottomface[3]; */
/*         bottomface[3] = temp; */
/*         temp = topface[1]; */
/*         topface[1] = topface[3]; */
/*         topface[3] = temp; */
/*     } */
/*     else if (ncpos == set<uint8_t>{0, 1, 4, 5}) { */
/*         bottomface.push_back(nc[0]); */
/*         bottomface.push_back(nc[4]); */
/*         bottomface.push_back(nc[5]); */
/*         bottomface.push_back(nc[1]); */
/*         topface.push_back(nc[3]); */
/*         topface.push_back(nc[7]); */
/*         topface.push_back(nc[6]); */
/*         topface.push_back(nc[2]); */
/*     } */
/*     else if (ncpos == set<uint8_t>{1, 2, 5, 6}) { */
/*         bottomface.push_back(nc[1]); */
/*         bottomface.push_back(nc[5]); */
/*         bottomface.push_back(nc[6]); */
/*         bottomface.push_back(nc[2]); */
/*         topface.push_back(nc[0]); */
/*         topface.push_back(nc[4]); */
/*         topface.push_back(nc[7]); */
/*         topface.push_back(nc[3]); */
/*     } */
/*     else if (ncpos == set<uint8_t>{2, 3, 6, 7}) { */
/*         bottomface.push_back(nc[2]); */
/*         bottomface.push_back(nc[6]); */
/*         bottomface.push_back(nc[7]); */
/*         bottomface.push_back(nc[3]); */
/*         topface.push_back(nc[1]); */
/*         topface.push_back(nc[5]); */
/*         topface.push_back(nc[4]); */
/*         topface.push_back(nc[0]); */
/*     } */
/*     else if (ncpos == set<uint8_t>{0, 3, 4, 7}) { */
/*         bottomface.push_back(nc[0]); */
/*         bottomface.push_back(nc[3]); */
/*         bottomface.push_back(nc[7]); */
/*         bottomface.push_back(nc[4]); */
/*         topface.push_back(nc[1]); */
/*         topface.push_back(nc[2]); */
/*         topface.push_back(nc[6]); */
/*         topface.push_back(nc[5]); */
/*     } */
/*     for (auto f : nfs) { */
/*         if (f == bf) continue; */
/*         for (auto x : f) { */
/*             for (int i=0; i<4; ++i) { */
/*                 if (topface[i] == x) */
/*                     nbfacepos.push_back(i); */
/*             } */
/*         } */
/*         int size = nbfacepos.size(); */
/*         if ((nbfacepos[size-1]+1)%4 == nbfacepos[size-2]) { */
/*             auto tmp = nbfacepos[size-1]; */
/*             nbfacepos[size-1] = nbfacepos[size-2]; */
/*             nbfacepos[size-2] = tmp; */
/*         } */
/*     } */
/* } */

#include "modu.h"
#include "msq.h"

struct resStat {
    /* int moduid; */
    int cellnum;
    bool success;
    int failat;//第?个单元失败
    int failnewcelltype;
    int failcase;//0是优化过一遍没有用，1是untangle失败，2是celltype为1
};
resStat work1(string filename) {
    Modu modu;
    modu.ReadTopo(filename);
    for (uint32_t i=0; i<modu.cnum; ++i) {
        modu.AddNewcell(i);
        modu.VtkOut();
        /*
         * 这里放优化函数
         */
        QualityImprover q(i+1);
        uint8_t v = q.CustomWrapper();
        if (v != 2) {
            resStat res = {modu.cnum, false, (int32_t)i, modu.newcelltype, 1};
            return res;
        }
        modu.VtkIn();
    }
    //可能进一步的优化
    modu.VtkOut();
    resStat res = {modu.cnum, true, modu.cnum, modu.newcelltype, 1};
    return res;
}

int main() {
    auto ans = work1(string("../meshfile/modu/con17.txt"));
    cout << ans.success << endl;
    return 0;
}

int mainkkk(int argc, char** argv) {
    ofstream fout;
    int maxcellnum = 11, maxmoduid = 145400;
    fout.open(to_string(maxcellnum) + "_modu_check_result.txt");
    string filename("../meshfile/modu/"+to_string(maxcellnum)+"_raw/");
    for (int i=1; i<maxmoduid; ++i) {
        if (i!=1387) continue;
        string f = filename + to_string(maxcellnum) + "_raw.txt_" + to_string(i);
        //string f("../meshfile/modu/con17.txt");
        auto ans = work1(f);
        fout << i << " " << ans.cellnum << " ";
        fout << ans.success << " " << ans.failat << " ";
        fout << ans.failnewcelltype << " " << ans.failcase << endl;
        //if (ans.success == true) {
            //string celln(to_string(ans.cellnum));
            //string moduid(to_string(i));//这里的moduid与modugenerate的topoid不是同一个东西
            //string cmd1;
            //cmd1 = "cp ./vtk/" + celln + "_tmp_opt.vtk ./result/" + moduid + "_" + celln + ".vtk";
            //system(cmd1.c_str());
        //}
        if (ans.success == false)
            cout << "error: " << i << " " << ans.cellnum << endl;
        if (ans.cellnum > 8)
            break;
        /* if (i%100 == 0) */
        /*     cout << i << endl; */
    }
    fout.close();
    return 0;
}

#include "msq.h"

QualityImprover::QualityImprover(uint8_t c){
    cellnum = c;
    return;
}
QualityImprover::~QualityImprover(){}
uint8_t QualityImprover::exec_output() {
    streambuf *backup;
    ofstream fout;
    fout.open("tmp.dat");
    backup = cout.rdbuf();
    uint8_t res = exec();
    cout.rdbuf(backup);
    fout.close();
    return res;
}
uint8_t QualityImprover::exec() {
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    Mesquite::MsqError err;
    Mesquite::MeshImpl mymesh;
    mymesh.read_vtk(filename.c_str(), err);
    if (err) return 0;

    Mesquite::ShapeImprovementWrapper wrapper;
    Mesquite::UntangleWrapper untangle;
    untangle.run_instructions(&mymesh, err);
    if (err) return 0;

    wrapper.run_instructions(&mymesh, err);
    if (err) return 0;

    Mesquite::QualityAssessor qa(false, false);
    Mesquite::InstructionQueue q;
    q.add_quality_assessor(&qa, err);
    Mesquite::MeshDomainAssoc mesh_and_domain = Mesquite::MeshDomainAssoc(&mymesh, 0);
    q.run_common(&mesh_and_domain, 0, 0, err);
    int inverted_elems = 0, inverted_samples = 0;
    qa.get_inverted_element_count(inverted_elems, inverted_samples, err);
    if (inverted_elems or inverted_samples) return 1;

    string outfilename = "./vtk/" + to_string(cellnum);
    outfilename += "_tmp_opt.vtk";
    mymesh.write_vtk(outfilename.c_str(), err);
    if (err) return 0;
    return 2;
}

bool Untangle(int cellnum) {
    string filename = "./vtk/" + to_string(cellnum);
    filename += "_tmp.vtk";
    Mesquite::MsqError err;
    Mesquite::MeshImpl mymesh;
    mymesh.read_vtk(filename.c_str(), err);

    Mesquite::UntangleWrapper untangle;
    streambuf *backup;
    ofstream fout;
    fout.open("tmp.dat");
    backup = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());
    untangle.run_instructions(&mymesh, err);

    Mesquite::QualityAssessor check_inverted(false, false);
    Mesquite::InstructionQueue q_invert_check;
    // a QuallityAssessor without a metric will just check for inverted elements and samples
    q_invert_check.add_quality_assessor(&check_inverted, err);
    Mesquite::MeshDomainAssoc mesh_and_domain = Mesquite::MeshDomainAssoc(&mymesh, 0);
    q_invert_check.run_common(&mesh_and_domain, 0, 0, err);
    int inverted_elems = 0, inverted_samples = 0;
    check_inverted.get_inverted_element_count(inverted_elems, inverted_samples, err);
    cout.rdbuf(backup);
    fout.close();
    if (inverted_elems || inverted_samples)
        return false;
    return true;
}
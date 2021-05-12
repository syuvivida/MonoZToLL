#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TH1D.h>
#include <TAttLine.h>
#include <TStyle.h>
using namespace std;
void DrawTau()
{
    TFile *file = new TFile("../output/test.root");
    TFile *file2 = new TFile("../output/test2.root");
    TFile *file3 = new TFile("../output/test3.root");
    TFile *file4 = new TFile("../output/test4.root");
    auto c1 = new TCanvas("c", "BPRE", 10,10,700,700);

    TH1D *var11 = ((TH1D *)file->Get("drtaue"));
    TH1D *var12 = ((TH1D *)file2->Get("drtaue"));
    TH1D *var13 = ((TH1D *)file3->Get("drtaue"));
    TH1D *var14 = ((TH1D *)file4->Get("drtaue"));
    /*
    var14->GetXaxis()->SetRangeUser(0,6);
    var14->GetXaxis()->SetTitle("numbers");
    var14->GetXaxis()->SetBinLabel(1, "0");
    var14->GetXaxis()->SetBinLabel(2, "1");
    var14->GetXaxis()->SetBinLabel(3, "2");
    var14->GetXaxis()->SetBinLabel(4, "3");
    var14->GetXaxis()->SetBinLabel(5, "4");
    var14->GetXaxis()->SetBinLabel(6, "5");
    */
    var11->GetXaxis()->SetRangeUser(0,4.0);
    var11->SetLineColor(kBlue);
    var12->SetLineColor(kOrange);
    var13->SetLineColor(kGray);
    var14->SetLineColor(5);

    var11->DrawNormalized();
    var12->DrawNormalized("same");
    var13->DrawNormalized("same");
    var14->DrawNormalized("same");

    c1->SaveAs("drtaue.png");
}
#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TH1D.h>
#include <TStyle.h>
using namespace std;

void simple_draw()
{
    TFile *file = new TFile("../output/lepton_result.root");
    auto c1 = new TCanvas("c", "BPRE", 10,10,700,700);
    THStack *hs = new THStack("hs","Number of lepton");
    TH1D *var1 = ((TH1D *)file->Get("elenumb"));
    TH1D *var2 = ((TH1D *)file->Get("munumb"));
    TH1D *var3 = ((TH1D *)file->Get("taunumb"));

    var1->GetXaxis()->SetRangeUser(0,3);
    //var1->GetXaxis()->SetTitle("numbers");
    var1->SetFillColor(kOrange+7);
    var1->GetXaxis()->SetBinLabel(1, "0");
    var1->GetXaxis()->SetBinLabel(2, "1");
    var1->GetXaxis()->SetBinLabel(3, "2");
    hs->Add(var1);
    
    var2->SetFillColor(49);
    hs->Add(var2);
    var3->SetFillColor(kOrange);
    hs->Add(var3);
    
    hs->Draw("nostackb&&text45");
    hs->GetXaxis()->SetTitle("numbers");
    TLegend *legend = new TLegend(0.55, 0.65, 0.76, 0.82);
    legend->AddEntry(var1, "electron", "f");
    legend->AddEntry(var2, "muon", "f");
    legend->AddEntry(var3, "tau", "f");
    legend->Draw();
    //var3->Draw("bar&&same  text0");
    c1->Modified();
    c1->SaveAs("number.png");


}

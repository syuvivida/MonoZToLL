#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TH1D.h>
#include <TAttLine.h>
#include <TStyle.h>
using namespace std;

void draw_zmass()
{
    TFile *file = new TFile("../output/test.root");
    auto c1 = new TCanvas("c", "BPRE", 10,10,700,700);
    TH1D *var1 = ((TH1D *)file->Get("drtaue"));
    TH1D *var2 = ((TH1D *)file->Get("elePT"));
    TH1D *var3 = ((TH1D *)file->Get("elePTPT"));
    TH1D *var4 = ((TH1D *)file->Get("elePTeta"));
    TH1D *var5 = ((TH1D *)file->Get("taunumb"));

/*
    var2->SetStats(0);

    var2->SetLineColor(kRed);
    var3->SetLineColor(kBlue);
    var4->SetLineColor(kBlack);
    //var4->SetLineStyle(8);
    var2->GetXaxis()->SetTitle("PT");

    var2->Draw();
    var3->Draw("same");
    var4->Draw("same");
*/
/*
    var5->GetXaxis()->SetTitle("numbers");
    var5->GetXaxis()->SetBinLabel(1, "0");
    var5->GetXaxis()->SetBinLabel(2, "1");
    var5->GetXaxis()->SetBinLabel(3, "2");
    var5->GetXaxis()->SetBinLabel(4, "3");
    var5->GetXaxis()->SetBinLabel(5, "4");
    var5->GetXaxis()->SetBinLabel(6, "5");
    var5->GetXaxis()->SetBinLabel(7, "6"); 
    var5->Draw();
*/ 
var1->GetXaxis()->SetTitle("dr");
var1->Draw();
    /*
    TLegend *legend = new TLegend(0.55, 0.65, 0.76, 0.82);
    legend->AddEntry(var2, "Original", "f");
    legend->AddEntry(var3, "After PT Cut", "f");
    legend->AddEntry(var4, "After eta cut", "f");
    legend->Draw();
    */
    c1->SaveAs("drtaue.png");
}

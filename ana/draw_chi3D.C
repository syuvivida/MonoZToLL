#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <TH1D.h>
#include <TAttLine.h>
#include <TStyle.h>
void draw_chi3D()
{
    //===========================
    //Total Weight(PT): 
    //  50~100 = 3728
    //  100~250 = 3869
    //  250~400 = 3739
    //  400~650 = 3713
    //  650~Inf = 4186
    //===========================
    TFile *Sig1 = new TFile("../output/DYHTMx2_1.root");
    TFile *Sig2 = new TFile("../output/DYHTMx2_50.root");
    TFile *Sig3 = new TFile("../output/DYHTMx2_150.root");
    TFile *Bg1 = new TFile("../output/DYHT70_100.root");
    TFile *Bg2 = new TFile("../output/DYHT100_200.root");
    TFile *Bg3 = new TFile("../output/DYHT200_400.root");
    TFile *Bg4 = new TFile("../output/DYHT400_600.root");
    TFile *Bg5 = new TFile("../output/DYHT600_800.root");
    TFile *Bg6 = new TFile("../output/DYHT800_1200.root");
    TFile *Bg7 = new TFile("../output/DYHT1200_2500.root");
    TFile *Bg8 = new TFile("../output/DYHT2500_Inf.root");

    TFile *BgPT1 = new TFile("../output/DYPT50_100.root");
    TFile *BgPT2 = new TFile("../output/DYPT100_250.root");
    TFile *BgPT3 = new TFile("../output/DYPT250_400.root");
    TFile *BgPT4 = new TFile("../output/DYPT400_650.root");
    TFile *BgPT5 = new TFile("../output/DYPT600_Inf.root");

    
    auto c1 = new TCanvas("c", "BPRE");//, 10,10,1000,1000);
    TH1D *Sig11_P = ((TH1D *)Sig1->Get("Chi3DPaperlog"));
    TH1D *Sig22_P = ((TH1D *)Sig2->Get("Chi3DPaperlog"));
    TH1D *Sig33_P = ((TH1D *)Sig3->Get("Chi3DPaperlog"));
    TH1D *Bg11_P = ((TH1D *)Bg1->Get("Chi3DPaperlog"));
    TH1D *Bg111_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg22_P = ((TH1D *)Bg2->Get("Chi3DPaperlog"));
    TH1D *Bg222_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg33_P = ((TH1D *)Bg3->Get("Chi3DPaperlog"));
    TH1D *Bg333_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg44_P = ((TH1D *)Bg4->Get("Chi3DPaperlog"));
    TH1D *Bg444_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg55_P = ((TH1D *)Bg5->Get("Chi3DPaperlog"));
    TH1D *Bg555_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg66_P = ((TH1D *)Bg6->Get("Chi3DPaperlog"));
    TH1D *Bg666_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg77_P = ((TH1D *)Bg7->Get("Chi3DPaperlog"));
    TH1D *Bg777_P = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg88_P = ((TH1D *)Bg8->Get("Chi3DPaperlog"));
    TH1D *Bg888_P = ((TH1D *)Bg1->Get("h_event"));

    TH1D *BgPT11_P = ((TH1D *)BgPT1->Get("Chi3DPaperlog"));
    TH1D *BgPT22_P = ((TH1D *)BgPT2->Get("Chi3DPaperlog"));
    TH1D *BgPT33_P = ((TH1D *)BgPT3->Get("Chi3DPaperlog"));
    TH1D *BgPT44_P = ((TH1D *)BgPT4->Get("Chi3DPaperlog"));
    TH1D *BgPT55_P = ((TH1D *)BgPT5->Get("Chi3DPaperlog"));

    double Bg111_event = Bg11_P->GetEntries();
    //cout<<"bb = "<<bb<<endl;
    Bg11_P->Scale(175.3/Bg111_event);
    double Bg222_event = Bg22_P->GetEntries();
    Bg22_P->Scale(147.4/Bg222_event);
    double Bg333_event = Bg33_P->GetEntries();
    Bg33_P->Scale(41.04/Bg333_event);
    double Bg444_event = Bg44_P->GetEntries();
    Bg44_P->Scale(5.674/Bg444_event);
    double Bg555_event = Bg55_P->GetEntries();
    Bg55_P->Scale(1.358/Bg555_event);
    double Bg666_event = Bg66_P->GetEntries();
    Bg66_P->Scale(0.6229/Bg666_event);
    double Bg777_event = Bg77_P->GetEntries();
    Bg77_P->Scale(0.1512/Bg777_event);
    double Bg888_event = Bg88_P->GetEntries();
    Bg88_P->Scale(0.003659/Bg888_event);

    BgPT11_P->Scale(344.3/3728);
    BgPT22_P->Scale(80.64/3869);
    BgPT33_P->Scale(2.955/3739);
    BgPT44_P->Scale(0.3807/3713);
    BgPT55_P->Scale(0.03711/4186);

    Sig11_P->SetStats(0);
    Sig11_P->GetXaxis()->SetTitle("log_{10}(IP^{3D}_{sig})");
    Sig11_P->GetXaxis()->SetRangeUser(-4,6);
    double Ymax, Ymin;
    Ymax=Bg88_P->GetYaxis()->GetXmax();
    Ymin=Bg88_P->GetYaxis()->GetXmin();
    
    //Sig11->SetMaximum(Ymax);
    //cout<<"Ymax = "<<Ymax<<endl;

    Sig11_P->SetLineWidth(3);
    Sig22_P->SetLineWidth(3);
    Sig33_P->SetLineWidth(3);

    Bg11_P->SetLineWidth(3);
    //Bg22->SetLineWidth(3);
    //Bg33->SetLineWidth(3);
    //Bg44->SetLineWidth(3);
    //Bg55->SetLineWidth(3);
    //Bg66->SetLineWidth(3);
    //Bg77->SetLineWidth(3);
    //Bg88->SetLineWidth(3);
    BgPT11_P->SetLineWidth(3);

    Sig11_P->SetLineColor(kRed);
    Sig22_P->SetLineColor(kGreen);
    Sig33_P->SetLineColor(kBlue);

    Bg11_P->SetLineColor(kYellow);
    //Bg22->SetLineColor(kBlack);
    //Bg33->SetLineColor(kCyan);
    //Bg44->SetLineColor(kOrange);
    //Bg55->SetLineColor(kSpring);
    //Bg66->SetLineColor(kTeal);
    //Bg77->SetLineColor(kAzure);
    //Bg88->SetLineColor(kViolet);
    BgPT11_P->SetLineColor(kBlack);

    
    Sig11_P->Scale(1.0/(float)Sig11_P->Integral());
    //Sig11->GetYaxis()->SetRangeUser(0,3e-1);
    c1->Divide(2,1);
    c1->cd(1);
    Sig11_P->SetTitle("3Dsig(0.01)");
    Sig11_P->SetMaximum(0.4);
    Sig11_P->Draw("hist");
    Sig22_P->DrawNormalized("hist&same");
    Sig33_P->DrawNormalized("hist&same");

    Bg11_P->Add(Bg22_P);
    Bg11_P->Add(Bg33_P);
    Bg11_P->Add(Bg44_P);
    Bg11_P->Add(Bg55_P);
    Bg11_P->Add(Bg66_P);
    Bg11_P->Add(Bg77_P);
    Bg11_P->Add(Bg88_P);

    BgPT11_P->Add(BgPT22_P);
    BgPT11_P->Add(BgPT33_P);
    BgPT11_P->Add(BgPT44_P);
    BgPT11_P->Add(BgPT55_P);

    Bg11_P->DrawNormalized("hist&same");
    //Bg22->DrawNormalized("hist&same");
    //Bg33->DrawNormalized("hist&same");
    //Bg44->DrawNormalized("hist&same");
    //Bg55->DrawNormalized("hist&same");
    //Bg66->DrawNormalized("hist&same");
    //Bg77->DrawNormalized("hist&same");
    //Bg88->DrawNormalized("hist&same");
    BgPT11_P->DrawNormalized("hist&same");
    
    TLegend *legend = new TLegend(0.10, 0.55, 0.35, 0.90);
    legend->AddEntry(Sig11_P, "ctau=1 m_{x^{2}}=1", "f");
    legend->AddEntry(Sig22_P, "ctau=10 m_{x^{2}}=50", "f");
    legend->AddEntry(Sig33_P, "ctau=1 m_{x^{2}}=150", "f");
    legend->AddEntry(Bg11_P, "DYJets_HTBin", "f");
    //legend->AddEntry(Bg11, "HT70 to 100", "f");
    //legend->AddEntry(Bg22, "HT100 to 200", "f");
    //legend->AddEntry(Bg33, "H200 to 400", "f");
    //legend->AddEntry(Bg44, "HT400 to 600", "f");
    //legend->AddEntry(Bg55, "HT600 to 800", "f");
    //legend->AddEntry(Bg66, "HT800 to 1200", "f");
    //legend->AddEntry(Bg77, "HT1200 to 2500", "f");
    //legend->AddEntry(Bg88, "HT2500 to INF", "f");
    legend->AddEntry(BgPT11_P, "DYJets_PTBin", "f");
    legend->Draw();
    c1->SetLogy();

    TH1D *Sig11 = ((TH1D *)Sig1->Get("Chi3Dlog"));
    TH1D *Sig22 = ((TH1D *)Sig2->Get("Chi3Dlog"));
    TH1D *Sig33 = ((TH1D *)Sig3->Get("Chi3Dlog"));
    TH1D *Bg11 = ((TH1D *)Bg1->Get("Chi3Dlog"));
    TH1D *Bg111 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg22 = ((TH1D *)Bg2->Get("Chi3Dlog"));
    TH1D *Bg222 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg33 = ((TH1D *)Bg3->Get("Chi3Dlog"));
    TH1D *Bg333 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg44 = ((TH1D *)Bg4->Get("Chi3Dlog"));
    TH1D *Bg444 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg55 = ((TH1D *)Bg5->Get("Chi3Dlog"));
    TH1D *Bg555 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg66 = ((TH1D *)Bg6->Get("Chi3Dlog"));
    TH1D *Bg666 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg77 = ((TH1D *)Bg7->Get("Chi3Dlog"));
    TH1D *Bg777 = ((TH1D *)Bg1->Get("h_event"));
    TH1D *Bg88 = ((TH1D *)Bg8->Get("Chi3Dlog"));
    TH1D *Bg888 = ((TH1D *)Bg1->Get("h_event"));

    TH1D *BgPT11 = ((TH1D *)BgPT1->Get("Chi3Dlog"));
    TH1D *BgPT22 = ((TH1D *)BgPT2->Get("Chi3Dlog"));
    TH1D *BgPT33 = ((TH1D *)BgPT3->Get("Chi3Dlog"));
    TH1D *BgPT44 = ((TH1D *)BgPT4->Get("Chi3Dlog"));
    TH1D *BgPT55 = ((TH1D *)BgPT5->Get("Chi3Dlog"));

    Bg11->Scale(175.3/Bg111_event);
    Bg22->Scale(147.4/Bg222_event);
    Bg33->Scale(41.04/Bg333_event);
    Bg44->Scale(5.674/Bg444_event);
    Bg55->Scale(1.358/Bg555_event);
    Bg66->Scale(0.6229/Bg666_event);
    Bg77->Scale(0.1512/Bg777_event);
    Bg88->Scale(0.003659/Bg888_event);

    Sig11->SetStats(0);
    Sig11->GetXaxis()->SetTitle("log_{10}(IP^{3D}_{sig})");
    Sig11->GetXaxis()->SetRangeUser(-4,6);
    //double Ymax, Ymin;
    //Ymax=Bg88_P->GetYaxis()->GetXmax();
    //Ymin=Bg88_P->GetYaxis()->GetXmin();

    Sig11->SetLineWidth(3);
    Sig22->SetLineWidth(3);
    Sig33->SetLineWidth(3);

    Bg11->SetLineWidth(3);
    BgPT11->SetLineWidth(3);

    Sig11->SetLineColor(kRed);
    Sig22->SetLineColor(kGreen);
    Sig33->SetLineColor(kBlue);
    Bg11->SetLineColor(kYellow);
    BgPT11->SetLineColor(kBlack);

    Sig11->Scale(1.0/(float)Sig11->Integral());
    //Sig11->GetYaxis()->SetRangeUser(0,3e-1);
    //c1->Divide(1,2);
    c1->cd(2);
    Sig11->SetTitle("3Dsig");
    Sig11->SetMaximum(0.4);
    Sig11->Draw("hist");
    Sig22->DrawNormalized("hist&same");
    Sig33->DrawNormalized("hist&same");

    Bg11->Add(Bg22);
    Bg11->Add(Bg33);
    Bg11->Add(Bg44);
    Bg11->Add(Bg55);
    Bg11->Add(Bg66);
    Bg11->Add(Bg77);
    Bg11->Add(Bg88);

    BgPT11->Add(BgPT22);
    BgPT11->Add(BgPT33);
    BgPT11->Add(BgPT44);
    BgPT11->Add(BgPT55);

    Bg11->DrawNormalized("hist&same");
    BgPT11->DrawNormalized("hist&same");
    TLegend *l2 = new TLegend(0.10, 0.55, 0.35, 0.90);
    l2->AddEntry(Sig11, "ctau=1 m_{x^{2}}=1", "f");
    l2->AddEntry(Sig22, "ctau=10 m_{x^{2}}=50", "f");
    l2->AddEntry(Sig33, "ctau=1 m_{x^{2}}=150", "f");
    l2->AddEntry(Bg11, "DYJets_HTBin", "f");
    l2->AddEntry(BgPT11, "DYJets_PTBin", "f");
    l2->Draw();


    c1->SaveAs("Chi3D.png");  
}
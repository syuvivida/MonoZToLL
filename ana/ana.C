#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
using namespace std;
void efferr(float nsig,float ntotal,float factor=1)
{
  float eff = nsig/ntotal;
  float err = sqrt( (1-eff)*eff/ntotal);
  cout << "efficiency = " << eff*factor << " +- " << err*factor << endl;
}
bool pt_greater(const TLorentzVector* a, const TLorentzVector* b){
	double A = a->Pt();
    double B = b->Pt();
    return (A > B);
}
void ana(){
    string outputfile = "../output/test.root";
    // out Tree branches
	TFile* fout = new TFile(outputfile.c_str(), "RECREATE" );
	TTree outTree("tree","out branches");
	//outTree.Branch("passTrigList", &passTrigList);  
	//outTree.Branch("prescaleList", &prescaleList);  
	//outTree.Branch("HT_MET", &HT_MET);  
	//outTree.Branch("HT_noMET", &HT_noMET);  
	//outTree.Branch("nPassTrig", &nPassTrig);  
    ifstream inputtxtFile("../ntuple_filelist/signal/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph.txt");
    string inputFile;
    TH1D *Z_massee = new TH1D("Z_massee", "Z->ee", 150, 0, 150);
    Long64_t NNTotal=0;
    Long64_t nPass[20]={0};
    while(getline(inputtxtFile,inputFile))
    {
        //std::string inputFile= "NCUGlobalTuples_1.root"
        TreeReader data(inputFile.data());
        Long64_t nTotal=0;
        Long64_t neeTotal=0;
        
        Long64_t nelePass[20]={0};
        for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++)
        {
            NNTotal++;
            data.GetEntry(jEntry);
            nTotal++;
            //0. has a good vertex
            int nVtx        = data.GetInt("nVtx");
            if(nVtx<1)continue;
            //1. trigger
            //2. electron
            int nEle = data.GetInt("nEle");
            TClonesArray* eleP4 = (TClonesArray*) data.GetPtrTObject("eleP4");
            vector<bool>& eleIsPassLoose = *((vector<bool>*) data.GetPtr("eleIsPassLoose"));
            vector<bool>& eleIsPassMedium = *((vector<bool>*) data.GetPtr("eleIsPassMedium"));
            vector<bool>& eleIsPassVeto = *((vector<bool>*) data.GetPtr("eleIsPassVeto"));
            vector<TLorentzVector*> myEles;
            myEles.clear();
            //bool findAZ=false;
            bool findEle[4]={false,false,false,false};
            for(int ie = 0; ie < nEle; ie++)
            {
                TLorentzVector* myEle = (TLorentzVector*)eleP4->At(ie);
                if(fabs(myEle->Eta())<2.5 )
                {
                    findEle[0]=true;
                }
                if(eleIsPassVeto[ie])
                {
                    findEle[1]=true;
                }
                if(eleIsPassMedium[ie])
                {
                    findEle[2]=true;
                    if(myEle->Pt()>20 )
                    {
                        findEle[3]=true;
                    }
                }
                if(findEle[0] && findEle[1]&&findEle[2]&&findEle[3])
                {
                    myEles.push_back(myEle);
                }
            }
            sort(myEles.begin(), myEles.end(), pt_greater);
            if(findEle[0])
            {
                nelePass[0]++;
            }
            if(findEle[1])
            {
                nelePass[1]++;
            }
            if(findEle[2])
            {
                nelePass[2]++;
            }
            if(findEle[3])
            {
                nelePass[3]++;
            }
            //cout<<"size of myEles is ="<<myEles.size()<<endl;
            //3. muon
            int nMu = data.GetInt("nMu");
            TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");
            vector<bool>& isTightMuon = *((vector<bool>*) data.GetPtr("isTightMuon"));
            vector<bool>& isSoftMuon = *((vector<bool>*) data.GetPtr("isSoftMuon"));
            vector<bool>& isGlobalMuon = *((vector<bool>*) data.GetPtr("isGlobalMuon"));
            vector<bool>& isTrackerMuon = *((vector<bool>*) data.GetPtr("isTrackerMuon"));
            float* mudz     = data.GetPtrFloat("mudz");
            float* mudxy     = data.GetPtrFloat("mudxy");
            float* muChi2NDF     = data.GetPtrFloat("muChi2NDF");
            int* muPixelHits   = data.GetPtrInt("muPixelHits");
            int* muTrkLayers   = data.GetPtrInt("muTrkLayers");
            int* muPixelLayers = data.GetPtrInt("muMatches");
            float* muChHadIso     = data.GetPtrFloat("muChHadIso");
            float* muNeHadIso     = data.GetPtrFloat("muNeHadIso");
            float* muGamIso     = data.GetPtrFloat("muGamIso");
            float* muPUPt     = data.GetPtrFloat("muPUPt");
            int* muHits = data.GetPtrInt("muHits");//vaild muon hits?
            int* muMatches = data.GetPtrInt("muMatches");//Matched Muon Stations?
            vector<int> myTMuos;
            vector<int> mySMuos;
            vector<int> myMuos;
            Double_t myMuIso;
            myTMuos.clear();
            mySMuos.clear();
            myMuos.clear();
            for(int im = 0; im < nMu; im++)
            {
                TLorentzVector* myMu = (TLorentzVector*)muP4->At(im);
                double mupT=myMu->Pt();
                myMuIso=(muChHadIso[im] +max(0.,muNeHadIso[im]+muGamIso[im]-0.5*muPUPt[im]))/mupT;
                if( fabs(myMu->Eta())>2.4 ) 
                {
                    continue;
                }
                if(muTrkLayers[im]<5)
                {
                    continue;
                }
                if(isTightMuon[im])
                {
                    if(isGlobalMuon[im])
                    {
                        if(myMu->Pt()>20 && muChi2NDF[im]<10 && muPixelHits[im]>0&&muHits[im]>0 &&muMatches[im]>1&&mudxy[im]<0.2&&mudz[im]<0.5&&myMuIso<=0.15)
                        {
                           myMuos.push_back(im);
                           continue;
                           //cout<<"0"<<endl;
                        }
                    }
                }
                if(isSoftMuon[im])
                {
                    if(isGlobalMuon[im]||isTrackerMuon[im])
                    {
                        if(myMu->Pt()>10&&muPixelLayers[im]>0&&mudxy[im]<0.3&&mudz[im]<20&&myMuIso<=0.25)
                        {
                            myMuos.push_back(im);
                        }
                    }
                }
            }
            //cout<<myMuos.size()<<endl;
            //cout<<"size of myMuos is ="<<myMuos.size()<<endl;
            //For lepton Selection
            bool Zee = false;
            if(myEles.size()>=2 && myMuos.size()>=2)
            {
                continue;
            }
            nPass[0]++;
            if (myEles.size()<2 && myMuos.size()<2)
            {
                continue;
            }
            nPass[1]++;
            if (myEles.size()>myMuos.size())
            {
                Zee=true;
                neeTotal++;
                nPass[1]++;
            }
            /*
            //4. veto tau (use to identified  and  rejected bg) 
            int nTau = data.GetInt("HPSTau_n");
            TClonesArray* tauP4 = (TClonesArray*) data.GetPtrTObject("HPSTau_4Momentum");
            vector<bool>& disc_decayModeFinding = *((vector<bool>*) data.GetPtr("disc_decayModeFinding"));// DecayModeFinding metho?
            vector<int> myTaus;
            myTaus.clear();
            for(int it=0; it < nTau; it++)
            {
                TLorentzVector* myTau = (TLorentzVector*)tauP4->At(it);
                if( myTau->Pt() < 18 )
                {
                    continue;
                }
                if( fabs(myTau->Eta())>2.3 )
                {
                    continue;
                }
                if( !disc_decayModeFinding[it] )
                {
                    continue;
                }
                myTaus.push_back(it);
            }
            if(myTaus.size()>0)
            {
                continue;
            }
            */
            //5. Z -> ee
            if(Zee)
            {
                double PT1, PT2;
                double PDGZmass=91.1876;
                double PT;
                double deltaMass;
                PT1=myEles[0]->Pt();
                PT2=myEles[1]->Pt();
                if(PT1>25&&PT2>20)
                {
                    TLorentzVector Z_boson_ee;
                    Z_boson_ee=*myEles[0]+*myEles[1];
                    //cout<<"total Mass ="<<MassLL<<endl;
                    deltaMass=abs(PDGZmass-Z_boson_ee.M());
                    //cout<<"deltaMass ="<<deltaMass<<endl;
                    if(deltaMass>15)continue;
                    Z_massee->Fill(Z_boson_ee.M());
                }
            }

            //cout<<"ele1 PT "<<myEles[0]->Pt()<<endl;
            //cout<<"ele2 PT "<<myEles[1]->Pt()<<endl;
            //Z_boson_ee.SetPxPyPzE(myEle1->Px()+myEle2->Px(),myEle1->Py()+myEle2->Py(),myEle1->Pz()+myEle2->Pz(),myEle1->E()+myEle2->E());         
            /*
            Z_boson_ee = *myEle1+*myEle2;

            Z_massee->Fill(Z_boson_ee.M());
            */
            //cout<<Z_boson_ee.M()<<endl;
            //AK4 Jet
            const int nTHINJets = data.GetInt("THINnJet");
            TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
            float* thinJetCSV =  data.GetPtrFloat("THINjetCISVV2");
            unsigned int nGoodTHINJets=0;
            std::vector<int> indexForDPhi;
            indexForDPhi.clear();
            for(int ij=0; ij < nTHINJets; ij++)
            {
                TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(ij);
                if(thisJet->Pt()<30)
                {
                    continue;
                }
                if(fabs(thisJet->Eta())>4.5)
                {
                    continue;
                }
                // for b-jet (medium ID)
                if(thinJetCSV[ij]<0.6328)//need check
                {
                    continue;
                }
                for(int jj=0; jj<ij; jj++)
                {
                    TLorentzVector* thatJet = (TLorentzVector*)thinjetP4->At(jj);
                    if(thatJet->Pt()<20)
                    {
                        continue;
                    }
                    if(fabs(thatJet->Eta())>2.4)
                    {
                        continue;
                    }
                }// end of inner loop jet
            } // end of outer loop let
            
            /*
            vector<vector<float>>& THINjetTrackPt = *((vector<vector<float>>*) data.GetPtr("THINjetTrackPt"));    
            for(int i=0; i<nTHINJets;i++)
            {
                for(int j=0;j<THINjetTrackPt[i].size();j++)
                {
                    cout<<"1"<<endl;
                }
            }*/
            //cout<<typeid(THINjetTrackPt).name()<<endl;
            /*
            for(unsigned int i=0; i<indexForDPhi.size(); i++)
            {
                int jetIndex=indexForDPhi[i];
                TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(jetIndex);
	            double dphi=TVector2::Phi_mpi_pi(pfMetPhi-thisJet->Phi());

            }
            */
        }// end of loop over entries
        cout << "neeTotal    = " << neeTotal << endl;
        cout << "nTotal    = " << nTotal << endl;
        //cout << "nelePass[" << 0 << "]= " << nelePass[0] << std::endl;
        for(int i=0;i<20;i++)
        if(nPass[i]>0)
            std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

        efferr(nelePass[10],neeTotal);
        cout<<"NNTotal ="<<NNTotal<<endl;
        cout<<"End of file"<<endl;
    }
    Z_massee->Write();
    fout->Close();
}
#include<bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

int line_num = 0;
// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

public:
    string Node_Name;  // Variable name
    vector<int> Children; // Children of a particular node - these are index of nodes in graph.
    vector<string> Parents; // Parents of a particular node- note these are names of parents
    int nvalues;  // Number of categories a variable represented by this node can take
    vector<string> values; // Categories of possible values
    vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    vector<float> numrtr;
    vector<float> denomtr;
    
    // Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
    {
        Node_Name=name;
    
        nvalues=n;
        values=vals;
    }
    string get_name()
    {
        return Node_Name;
    }
    vector<int> get_children()
    {
        return Children;
    }
    vector<string> get_Parents()
    {
        return Parents;
    }
    vector<float> get_CPT()
    {
        return CPT;
    }
    int get_nvalues()
    {
        return nvalues;
    }
    vector<string> get_values()
    {
        return values;
    }
    float get_prob(int i)
    {
        float ab = (numrtr[i] + 0.0)/denomtr[i];
        return ab;
    }
    void set_CPT(vector<float> new_CPT)
    {
        CPT.clear();
        CPT=new_CPT;
        for(int i=0;i<CPT.size();i++){
            numrtr.push_back(0.0);
            denomtr.push_back(0.0);
        }
    }
    void init_prob(){
        for(int i=0;i<CPT.size();i++){
            numrtr[i] += 0.01;
            denomtr[i] += 0.01*nvalues;
        }
    }
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }
    int val_index(string a){
        for(int i=0;i<nvalues;i++){
            if(a.compare(values[i])==0){
                return i;
            }
        }
        return -1;
    }


};


// The whole network represted as a list of nodes
class network{

public:
    vector<Graph_Node> Pres_Graph;
    vector< vector<int> > v_glbl;
    int addNode(Graph_Node node)
    {
        Pres_Graph.push_back(node);
        return 0;
    }
    
    
    int netSize()
    {
        return Pres_Graph.size();
    }
    // get the index of node with a given name
    int get_index(string val_name)
    {
        for(int i=0;i<Pres_Graph.size();i++)
        {
            if(Pres_Graph[i].Node_Name.compare(val_name)==0){
                return i;
            }
        }
        return -1;
    }
    Graph_Node *get_nth_node(int n)
    {
        return &Pres_Graph[n];
    }
    // get the iterator of a node with a given name
    Graph_Node *search_node(string val_name)
    {
        int ind = get_index(val_name);
        return &Pres_Graph[ind];
    }
    int get_arr_point(int val, vector<int> Prnts, vector<int> values){
        int mult = 1;
        int sum = 0;
        for(int i=Prnts.size()-1; i>=0;i--){
            Graph_Node *par = get_nth_node(Prnts[i]);
            int my_val = values[i];
            sum = sum + mult*my_val;
            mult = mult*par->nvalues;
        }
        return val*mult + sum;
    }
    float get_par_prob(int val, Graph_Node *gn, vector<int> curr_val){
        vector<string> Parents = gn->get_Parents();
        vector<int> Prnts;
        vector<int> values;
        for(int i=0;i<Parents.size();i++){
            int par_ind = get_index(Parents[i]);
            Prnts.push_back(par_ind);
            values.push_back(curr_val[par_ind]);
        }
        int ind = get_arr_point(val, Prnts, values);
        return gn->get_prob(ind);
    }
    int predict_value(int gn_ind, vector<int> curr_val){
        Graph_Node *gn = get_nth_node(gn_ind);
        vector<float> prob;
        float sum = 0.0;
        int nvalues = gn->get_nvalues();
        for(int i=0;i<nvalues;i++){
            float prod = get_par_prob(i, gn, curr_val);
            vector<int> Children = gn->get_children();
            for(int j=0;j<Children.size();j++){
                int cl_val = curr_val[Children[j]];
                Graph_Node *child = get_nth_node(Children[j]);
                float cl_prob = get_par_prob(cl_val, child, curr_val);
                prod = prod*cl_prob;
            }
            prob.push_back(sum + prod);
            sum = sum + prod;
        }
        float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/sum));
        for(int i=0;i<nvalues;i++){
            if(r2<=prob[i]){
                return i;
            }
        }
        return nvalues-1;
    }
    void create_cell(vector<int> depth, vector<int> req, int ind, int num_par){
        if(depth.size()!=num_par){
            Graph_Node *me = get_nth_node(ind);
            string par_str = me->get_Parents()[depth.size()];
            int par_ind = get_index(par_str);
            Graph_Node *parent = get_nth_node(par_ind);
            depth.push_back(-1);
            req.push_back(-1);
            int dpt_size = depth.size();
            for(int i=0;i<parent->nvalues;i++){
                depth[dpt_size-1] = par_ind;
                req[dpt_size-1] = i;
                create_cell(depth, req, ind, num_par);
            }
        }
        else{
            Graph_Node *me = get_nth_node(ind);
            for(int i=0;i<me->nvalues;i++){
                int arr_p = get_arr_point(i, depth, req);
                for(int j=0;j<v_glbl.size();j++){
                    int flag = 1;
                    for(int l=0;l<depth.size();l++){
                        if(v_glbl[j][depth[l]]!=req[l]){
                            flag = 0;
                            break;
                        }
                    }
                    if(flag==1){
                        if(v_glbl[j][ind]==i){
                            me->numrtr[arr_p] += 1.0;
                        }
                        me->denomtr[arr_p] += 1.0;
                    }
                }
            }
        }
    }
    void change_cell(vector<int> new_val, vector<int> old_val, int ind){
        Graph_Node *pr = get_nth_node(ind);
        vector<int> my_prnts;
        vector<int> my_par_val;
        vector<string> my_Parents = pr->get_Parents();
        for(int j=0;j<my_Parents.size();j++){
            int par_ind = get_index(my_Parents[j]);
            my_prnts.push_back(par_ind);
            my_par_val.push_back(old_val[par_ind]);
        }
        int new_pnt = get_arr_point(new_val[ind], my_prnts, my_par_val);
        int old_pnt = get_arr_point(old_val[ind], my_prnts, my_par_val);
        pr->numrtr[new_pnt] += 1.0;
        pr->numrtr[old_pnt] -= 1.0;
        vector<int> Children = pr->get_children();
        for(int i=0;i<Children.size();i++){
            Graph_Node *child = get_nth_node(Children[i]);
            vector<int> prnts;
            vector<int> old_vals;
            vector<int> new_vals;

            vector<string> Parents = child->get_Parents();
            for(int j=0;j<Parents.size();j++){
                int par_ind = get_index(Parents[j]);
                prnts.push_back(par_ind);                
                old_vals.push_back(old_val[par_ind]);                
                new_vals.push_back(new_val[par_ind]);
            }
            for(int j=0;j<child->nvalues;j++){
                int arr_ind = get_arr_point(j, prnts, old_vals);
                if(j==old_val[Children[i]]){
                    child->numrtr[arr_ind] -= 1.0;
                }
                child->denomtr[arr_ind] -= 1.0;
                int arr_ind2 = get_arr_point(j, prnts, new_vals);
                if(j==new_val[Children[i]]){
                    child->numrtr[arr_ind2] += 1.0;
                }
                child->denomtr[arr_ind2] += 1.0;       
            }
        }
    }    
    void main_fnc(vector< vector<string> > input, time_t start, int time_sec){
        int lng = input.size();
        vector<int> missing;
        for(int i=0;i<lng;i++){
            vector <int> tmp;
            for(int j=0;j<input[0].size();j++){
                int tp = Pres_Graph[j].val_index(input[i][j]);
                if(tp==-1){
                    missing.push_back(j);
                    tp = 0;
                }
                tmp.push_back(tp);
            }
            v_glbl.push_back(tmp);
        }
        for(int i=0;i<input[0].size();i++){
            vector<int> dpt;
            vector<int> req;
            Graph_Node *gp = get_nth_node(i);
            int num_par = gp->get_Parents().size();
            create_cell(dpt, req, i, num_par);
        }
        for(int i=0;i<input[0].size();i++){
            Graph_Node *gp = get_nth_node(i);
            gp->init_prob();
        }
        vector< vector<int> > v_temp = v_glbl;
        int it = 0;
        while(true){
            time_t now = static_cast <unsigned> (time(NULL));
            if(difftime(now, start)>time_sec){
                break;
            }
            if((it+1)%10==0){
                cout<<"Epoch:"<<it+1<<endl;
            }
            for(int i=0;i<lng;i++){
                v_temp[i][missing[i]] = v_glbl[i][missing[i]];
                v_glbl[i][missing[i]] = predict_value(missing[i], v_glbl[i]);
            }
            for(int i=0;i<lng;i++){
                if(v_temp[i][missing[i]]!=v_glbl[i][missing[i]]){
                    change_cell(v_glbl[i], v_temp[i], missing[i]);
                }
            }
            it = it+1;
        }
    }
};

network read_network(char *filename)
{
    network Alarm;
    string line;
    int find=0;
    ifstream myfile(filename); 
    string temp;
    string name;
    vector<string> values;
    
    if (myfile.is_open())
    {
        while (! myfile.eof() )
        {
            line_num += 1;
            stringstream ss;
            getline (myfile,line);
            
            
            ss.str(line);
            ss>>temp;
            
            
            if(temp.compare("variable")==0)
            {
                    
                    ss>>name;
                    line_num += 1;
                    getline (myfile,line);
                   
                    stringstream ss2;
                    ss2.str(line);
                    for(int i=0;i<4;i++)
                    {
                        ss2>>temp;
                    }
                    values.clear();
                    while(temp.compare("};")!=0)
                    {
                        values.push_back(temp);
                        
                        ss2>>temp;
                    }
                    Graph_Node new_node(name,values.size(),values);
                    int pos=Alarm.addNode(new_node);

                    
            }
            else if(temp.compare("probability")==0)
            {
                    
                    ss>>temp;
                    ss>>temp;
                    
                    Graph_Node *listIt;
                    Graph_Node *listIt1;
                    listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
                    while(temp.compare(")")!=0)
                    {
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
                        values.push_back(temp);
                        
                        ss>>temp;

                    }
                    listIt->set_Parents(values);
                    line_num += 1;
                    getline (myfile,line);
                    stringstream ss2;
                    
                    ss2.str(line);
                    ss2>> temp;
                    
                    ss2>> temp;
                    
                    vector<float> curr_CPT;
                    string::size_type sz;
                    while(temp.compare(";")!=0)
                    {
                        curr_CPT.push_back(atof(temp.c_str()));
                        ss2>>temp;
                    }
                    listIt->set_CPT(curr_CPT);
            }
        }
        if(find==1)
        myfile.close();
    }
    
    return Alarm;
}

vector< vector<string> > read_samples(char *filename)
{
    vector< vector<string> > Data;
    string line;
    int find=0;
    ifstream myfile(filename); 
    string temp;
    
    if (myfile.is_open())
    {
        while (! myfile.eof() )
        {
            stringstream ss;
            getline (myfile,line);
            
            vector<string> tp;
            ss.str(line);
            while(ss>>temp){
                tp.push_back(temp);
            }
            Data.push_back(tp);

        }
    }
    myfile.close();
    return Data;
}

// void print_network(char* filename,int line_count)
// {
//     int counter = 0;
//     string line;
//     int find=0;
//     ifstream myfile(filename);
//     string temp;
//     string name;
//     vector<string> values;
//     if (myfile.is_open())
//     {
//       while (! myfile.eof() )
//       {
//             counter++;
//           stringstream ss;
//         getline (myfile,line);
//             if(counter==line_count)
//                 cout << line;
//             else
//                 cout << line << '\n';
//         ss.str(line);
//            ss>>temp;
//            if(temp.compare("variable")==0)
//            {
//                  ss>>name;
//                  getline (myfile,line);
//                 cout << line<<'\n';
//            }
//            else if(temp.compare("probability")==0)
//            {
//                  ss>>temp;
//                  ss>>temp;
//         int index;
//                  index=Alarm.get_index(temp);
//                 getline (myfile,line);
//                  stringstream ss2;
//                  ss2.str(line);
//                  ss2>> temp;
//                 cout << temp << " ";
//                  ss2>> temp;
//                 int j=0;
//                  while(temp.compare(";")!=0)
//                  {
//                     cout << fixed << setprecision(4);
//                     if(Alarm.Pres_Graph[index].final_CPT[j]!=0)
//                         cout << Alarm.Pres_Graph[index].final_CPT[j]<<" ";
//                     else
//                         cout << "0.0001 ";
//                     j++;
//                      ss2>>temp;
//                 }
//                 cout << temp << '\n';
//            }
//       else
//       {
//       }
//       }
//       if(find==1)
//       myfile.close();
//     }
// }

void print_network(network Alarm, char *filename)
{
    string line;
    int find=0;
    ifstream myfile(filename);
    ofstream outfile("solved_alarm.bif");
    string temp;
    string name;
    int write_num = 0;
    
    if (myfile.is_open())
    {
        while (! myfile.eof() )
        {
            write_num += 1;
            stringstream ss;
            getline (myfile,line);
            
            
            ss.str(line);
            if(ss>>temp){

            }
            else{
                continue;
            }
            if(temp.compare("probability")==0)
            {
                outfile<<line;
                outfile<<endl;
                ss>>temp;
                ss>>temp;
                
                Graph_Node *listIt;
                listIt=Alarm.search_node(temp);
                int index=Alarm.get_index(temp);
                ss>>temp;
                while(temp.compare(")")!=0)
                {
                    ss>>temp;
                }
                getline (myfile,line);
                stringstream ss2;
                
                ss2.str(line);
                ss2>> temp;
                outfile<<"\t"<<temp;
                
                ss2>> temp;
                int iter = 0;
                while(temp.compare(";")!=0)
                {
                    if (listIt->get_prob(iter)<0.0001){
                        outfile<<" 0.0001";
                    }
                    else{
                        outfile<<" "<< fixed << setprecision(4) << listIt->get_prob(iter);
                    }
                    iter++;
                    ss2>>temp;
                }
                outfile<<" "<<temp;
            }
            else{
                outfile<<line;
            }
            if(write_num==line_num){

            }
            else{
                outfile<<endl;
            }
        }
        if(find==1)
        myfile.close();
    }
}

int main(int argc, char *argv[])
{
    srand (static_cast <unsigned> (time(0)));
    time_t start = static_cast <unsigned> (time(0));
    network Alarm;
    Alarm=read_network(argv[1]);
    vector< vector<string> > sample = read_samples(argv[2]);
    int time_sec = 550;
    Alarm.main_fnc(sample, start, time_sec);
    print_network(Alarm, argv[1]);
    time_t end = static_cast <unsigned> (time(0));
}

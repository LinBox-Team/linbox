#include <iostream>

#include <vector>
#include <queue>

#include "linbox-config.h"
#include "linbox/integer.h"


using namespace std;
namespace LinBox {

template <class Element>
class ijElement {

public:
	size_t i;
	size_t j;
	Element x;

	ijElement(size_t ii, size_t jj, Element xx) {i=ii;j=jj;x=xx;}
	ijElement(ijElement& Y) {i = Y.i; j = Y.j; x = Y.x;}
	~ijElement() {};
	//Element& setElement(Element& y) {return x=y;}	
};

template <class Element>
class GridElement {

public: 
	GridElement* prev;
	GridElement* next;
	GridElement* down;
	GridElement* up;
	ijElement<Element> X;

	GridElement(ijElement<Element>& Y): X(Y) {
                prev = NULL;
                next = NULL;
                down = NULL;
                up = NULL;	
	} 
	GridElement(size_t i, size_t j, Element x): X(i,j,x) {
		prev = NULL;
		next = NULL;
		down = NULL;
		up = NULL; 
	}
	~GridElement() {
		//prev = NULL; next = NULL; down = NULL; up = NUll;
	}

	Element& getX() {return X.x;}
	size_t getI() {return X.i;}
	size_t getJ() {return X.j;}

	Element& setElement(Element& y) {return X.x=y;}
	ijElement<Element>& setijElement(size_t i, size_t j, Element& x) {X.i=i; X.j = j; X.x = x; return X;}
};

template <class Field, class Element>
class Grid {

public:
	Field _F;
	size_t _n;
	size_t _m;
	vector<int> rowOcc;
	//vector<Element> rowGcd;
	vector<int> colOcc;
	//vector<Element> colGcd;

	//bool comp(size_t x, size_t y) const {if (rowOcc[x]<rowOcc[y]) return true; else return false;}

	queue<size_t> Q; //change to priority queue?
	//priority_queue<size_t> QQ(comp);	
	vector<GridElement<Element>*> A; 	//vector of row heads
	vector<GridElement<Element>*> AT;	//vector of column heads
	

/* 
 * reads the matrix from the file, deletes mC[i]=1 columns
 * sets row/col Occ and Gcd
 * file is sorted in sms format	
 */
	Grid (Field F, istream& in, vector<int>& mR, vector<int>& mC): _F(F) {
		read(F, in, mR, mC); 
		in >> _m >> _n;
/*
//int t= _m;
//_m = _n;
//_n = t;
		rowOcc.resize(_m);
		colOcc.resize(_n);
		//rowGcd.resize(_m,0);
		//colGcd.resize(_n,0);
		A.resize(_m, NULL);
		AT.resize(_n, NULL);
	
		mR.resize(_m);
		mC.resize(_n);
	
		char c;
		do in >> c; while (isspace (c));
		
		vector<GridElement<Element>*> ends(_n, NULL);

        	if (c == 'M') {
                	size_t i, i_prev, j, j_prev;
                	i_prev = 0;
                	j_prev = 0;

                	Element x, x_prev; x_prev = 0; x=0;
                	while (in >> i) {
                        	in >> j;

//int t = i; i=j; j=t;
                        	if ( (i > _m) || (j > _n) ) {
                                	cout << "InvalidMatrixInput \n"<< flush;
					return;
				}
                        	F.read(in, x);
                        	if ((j > 0) && (F.isZero(x))) continue;
                        	if ((j > 0) && (mC[j-1]==1)){
                                	continue;       //mC are to be ignored;
                        	}
                        	//std::cout << i << " " <<  j << " " << x << "\n";
                        	if ((i!=0) && (j !=0 )) {
					ijElement<Element> X(i-1,j-1,x);
					ends[j-1] = addElement(ends[j-1], X);
				}
                        	//if ((i != i_prev)) {
                                	//if ((i_prev>0) && (rowOcc[i_prev-1]==1)) {
                                        ////cout << "1 row" << i_prev << "\n";
					//	cout << "Adding " << i_prev-1 << " to Q\n" << flush;
                                        //	Q.push(i_prev-1);
                                	//}
                                	//size_t i_max = i;
                                	//size_t i_min = i_prev+1;
                                	//if (i_max==0) i_max = _m+1;
                                	//for (size_t k=i_min; k < i_max;++k) {
                                        ////cout << "0 rows" << k << "\n";
                                        //	mR[k-1] = 2;
                                	//}
                                	//i_prev = i;
                        	//}
                        	if ((i==0) && (j ==0 )) break;
			}
			for (int k=0; k < rowOcc.size(); ++k) {
				//if (rowOcc[k]>0) cout << k+1 << " " <<rowGcd[k]<< "\n" << flush;
				if ((rowOcc[k]==1) && (colGcd[A[k]->getJ()]==abs(A[k]->getX()))) {
                                	//cout << "1 row" << i_prev << "\n";
                                       	cout << "Adding " << k+1 << " to Q\n" << flush;
                                        Q.push(k);
                               	}
			}

			//for (int k=0; k < colOcc.size(); ++k) {
			//	if (colOcc[k]>0) cout << k+1 << " " << colGcd[k]<< "\n" << flush;
			//}

                }
		mC.clear();
		mC.resize(_n,0);
*/
	}

	void read(Field F, istream& in, vector<int>& mR, vector<int>& mC) {
                in >> _m >> _n;
/* !!! */
//int t= _m;
//_m = _n;
//_n = t;
                rowOcc.resize(_m,0);
                colOcc.resize(_n,0);
                //rowGcd.resize(_m,0);
                //colGcd.resize(_n,0);
                A.resize(_m, NULL);
                AT.resize(_n, NULL);

                mR.resize(_m);
                mC.resize(_n);
                char c;
                do in >> c; while (isspace (c));

                vector<GridElement<Element>*> ends(_n, NULL);

                if (c == 'M') {
                        size_t i, i_prev, j, j_prev;
                        i_prev = 0;
                        j_prev = 0;

                        Element x, x_prev; x_prev = 0; x=0;
                        while (in >> i) {
                                in >> j;
/* !!! */
//int t = i; i=j; j=t;
                                if ( (i > _m) || (j > _n) ) {
                                        cout << "InvalidMatrixInput \n"<< flush;
                                        return;
                                }
                                F.read(in, x);
                                if ((j > 0) && (F.isZero(x))) continue;
                                if ((j > 0) && (mC[j-1]==1)){
                                        continue;       //mC are to be ignored;
                                }
                                //std::cout << i << " " <<  j << " " << x << "\n";
                                if ((i!=0) && (j !=0 )) {
                                        ijElement<Element> X(i-1,j-1,x);
                                        ends[j-1] = addElement(ends[j-1], X);
                                }
                                //if ((i != i_prev)) {
                                        //if ((i_prev>0) && (rowOcc[i_prev-1]==1)) {
                                        ////cout << "1 row" << i_prev << "\n";
                                        //      cout << "Adding " << i_prev-1 << " to Q\n" << flush;
                                        //      Q.push(i_prev-1);
                                        //}
                                        //size_t i_max = i;
                                        //size_t i_min = i_prev+1;
                                        //if (i_max==0) i_max = _m+1;
                                        //for (size_t k=i_min; k < i_max;++k) {
                                        ////cout << "0 rows" << k << "\n";
                                        //      mR[k-1] = 2;
                                        //}
                                        //i_prev = i;
                                //}
                                if ((i==0) && (j ==0 )) break;
                        }
                        for (int k=0; k < rowOcc.size(); ++k) {
                                //if (rowOcc[k]>0) cout << k+1 << " " <<rowGcd[k]<< "\n" << flush;
                                if ((rowOcc[k]==1) && (/*colGcd[A[k]->getJ()]*/1==abs(A[k]->getX()))) {
                                        //cout << "1 row" << i_prev << "\n";
                                        cout << "Adding " << k+1 << " to Q\n" << flush;
                                        Q.push(k);
                                }
                        }
/*
                        for (int k=0; k < colOcc.size(); ++k) {
                                if (colOcc[k]>0) cout << k+1 << " " << colGcd[k]<< "\n" << flush;
                        }
*/
                }
                mC.clear();
                mC.resize(_n,0);

	}

/*
 * deletes the non-null element Aij
 */
	GridElement<Element>* deleteElement(GridElement<Element>* Aij) {
		if (Aij->prev != NULL) Aij->prev->next = Aij->next;
		else A[Aij->getI()] = Aij->next;
		if (Aij->next != NULL) Aij->next->prev = Aij->prev;
		if (Aij->down != NULL) Aij->down->up = Aij->up;
		else AT[Aij->getJ()] = Aij->up;
		if (Aij->up != NULL)   Aij->up->down = Aij->down;	
		--rowOcc[Aij->getI()];
		if (rowOcc[Aij->getI()]==1) {
			//cout << "Adding " << Aij->getI()+1 << " to Q\n" << flush;
			if (1/*colGcd[A[Aij->getI()]->getJ()]*/ == abs (A[Aij->getI()]->getX()))
				Q.push(Aij->getI());
		}
		--colOcc[Aij->getJ()];
		GridElement<Element>* tmp = Aij->up; 
		delete Aij;
		return tmp;
	} 

	void deleteColumn (size_t j) {
		colOcc[j] = 0;
		GridElement<Element>* tmp = AT[j];
		while (tmp != NULL) {
			--rowOcc[tmp->getI()];
			if (rowOcc[tmp->getI()] ==1) {
				cout << "Adding " << tmp->getI()+1 << " to Q\n" << flush;
				if ( 1/*colGcd[A[tmp->getI()]->getJ()]*/ == abs (A[tmp->getI()]->getX()))
					Q.push(tmp->getI());
			}
			if (tmp->prev != NULL) tmp->prev->next = tmp->next;
			else A[tmp->getI()] = tmp->next;
			if (tmp->next != NULL) tmp->next->prev = tmp->prev;
			GridElement<Element>* tmp2 = tmp->up;
			delete tmp;
			tmp = tmp2;
		}		
	}

        void deleteRow (size_t i) {
		rowOcc[i] = 0;
                GridElement<Element>* tmp = A[i];
                while (tmp != NULL) {
			--colOcc[tmp->getJ()];
                        if (tmp->down != NULL) tmp->down->up = tmp->up;
                        else AT[tmp->getJ()] = tmp->up;
                        if (tmp->up != NULL) tmp->up->down = tmp->down;
                        GridElement<Element>* tmp2 = tmp->next;
                        delete tmp;
                        tmp = tmp2;
                }
        }	

/* 
 * add a new Grid Element after (at ''up'') of the given GridElement
 * if lower==NULL Y should be the lowest element of the column	
 * returns a pointer to the new element
 */
	GridElement<Element>* addElement(GridElement<Element>* lower, ijElement<Element>& Y) {
		//if (lower->getJ() != Y.j) {
		//	cout << "dupa\n" << flush;
		//} else if (lower->getI() >= Y.i) {
		//	cout << "dupa2\n" << flush;
		//}
		//cout << "Add " << Y.i << Y.j << Y.x ;
		//if (lower != NULL) cout << " after "<< lower->getI() << lower->getJ() << lower->getX() << "\n";
		//else cout << " at begining\n";
		++rowOcc[Y.i];
		++colOcc[Y.j];
		//rowGcd[Y.i]=gcd(rowGcd[Y.i], Y.x);
		//colGcd[Y.j]=gcd(colGcd[Y.j], Y.x);
		GridElement<Element>* X = new GridElement<Element>(Y);
		if (lower != NULL) {
			GridElement<Element>* tmp = lower->up;
			lower->up = X;
			X->down = lower;
			X->up = tmp;
			if (tmp != NULL) tmp->down = X;
			tmp = A[Y.i];
			A[Y.i] = X;
			X->next = tmp;
			if (tmp != NULL) tmp->prev = X;
		} else {
			GridElement<Element>* tmp = AT[Y.j];
			AT[Y.j] = X;
			X->up = tmp;
			if (tmp != NULL) tmp->down = X;	
			tmp = A[Y.i];
                        A[Y.i] = X;
                        X->next = tmp;
                        if (tmp != NULL) tmp->prev = X;
		}
		return X;
	}

/* 
 * recursive procedure to reduce the grid
 * returns precomputed rank
 */

	int reduce(int& rank, int S, vector<int>& mR, vector<int>& mC, ostream& os) {
		cout << "rank at begin reduce " << rank << "\n" << flush;
	        while (!Q.empty()) {
			size_t i=Q.front();
                	Q.pop();  
	               	if (rowOcc[i]==1) {//if row not deleted 
	                	size_t j = A[i]->getJ();
                               	Element x; _F.init(x, A[i]->getX());
                               	if (mC[j]==1) {//if col deleted (not needed as we delete at once)
                                	mR[i]=2;
                               	} 
                               	else if (abs(x)==1/*colGcd[j]*/) {
	                        	//cout << "Row/column "<< i+1 << "," << j+1<< "," << x << "to reduce\n" <<flush;
				       	if (abs(x) > 1) cout << "adds " << x << "to the diagonal\n"<< flush;
	                               	if (mC[j] !=1 ) {//if col not deleted then delete, update rank
	                                	++rank; 
		                               	mC[j]=1;
                                               	mR[i]=1; 
						deleteColumn(j);//do not mark 0 rows
						//deleteRow(i);//not needed						
                                        }
                                } //else cout << "NOT Row/column "<< i+1 << "," << j+1<< "," << x << "to reduce\n" <<flush;
			}
		}

		cout << "Rank at end reduce/begin elimination" << rank <<"\n" << flush;
        
		size_t ini=0;
        	bool pivotFound;

        	size_t row2;
        	size_t j1,j2;
        	Element x,x1,x2;
        	x1=1;x2=1;x=0;

        	//init = rank;
        	while (1) {
                	pivotFound =false;
                	if (ini>=mR.size()) ini=0;
                	for (size_t i=ini; i <  mR.size(); ++i) {
                                if (rowOcc[i]==2) {
                                        //cout << "2 row found " << i <<"\n" << flush;
                                        j1 = A[i]->getJ();
                                        _F.init(x1,A[i]->getX());
                                        j2 = A[i]->next->getJ();
                                        _F.init(x2,A[i]->next->getX());
                                        //if (x2%x1==0) {
                                        //if (abs(x1)==1) {//For Z
                                        if ((abs(x1)==1/*rowGcd[i]*/) && (abs(x1)== 1/*colGcd[j1]*/)) {
                                                if ((abs(x2)== 1/*rowGcd[i]*/) && (abs(x2)==1/*colGcd[j2]*/) && (colOcc[j1]> colOcc[j2])) {
                                                        size_t jj = j1;
                                                        Element xx = x1;
                                                        j1 =j2;x1=x2;
                                                        j2 = jj;x2=xx;
                                                }
                                                pivotFound = true;
                                                row2=i;
                                                ini = i+1; break;
                                        } else
                                                if ((abs(x2)==1/*rowGcd[i]*/) && (abs(x2)== 1/*colGcd[j2]*/)) {
                                                size_t jj = j1;
                                                Element xx = x1;
                                                j1 =j2;x1=x2;
                                                j2 = jj;x2=xx;
                                                pivotFound = true;
                                                row2=i;
                                                ini = i+1;break;
                                        }

                                }
                        }
                
                	//pivotFound =false;
                	if (pivotFound) {
                        	if (abs(x1)>1) cout << "adds " << x1 << "to the diagonal\n" << flush;
                        	//cout << "rank before elimination " << rank << "\n";
                        	//int init = rank;
                        	mC[j1]=1;
                        	++rank;
                        	mR[row2]=1;
                        	//A[row2].clear();
                        	//Asize[row2]=0;
                        	_F.init(x, -x2/x1);
                        	//cout << "found " << j1 << "," << x2 << "," << j2 << "," << x2 << "\n"<< flush;
                        	////cout << "reducing column " << j2+1 << " by  (" << j1+1 << "," << x << "}\n" << flush;
                        	//cout << "row " << row2 << "\n"<<flush;

				GridElement<Element>* p1=AT[j1];
				GridElement<Element>* p2=NULL;
				GridElement<Element>* p2next=AT[j2];

				while (p1 != NULL) {
					while (p2next != NULL) {
						if (p2next->getI() >= p1->getI()) break;
						p2 = p2next;
						p2next = p2next->up;
					}
					Element y; _F.init(y, p1->getX());
					Element z;
					if ((p2next!= NULL) && (p2next->getI()==p1->getI()) ) {
						//update
						//cout << "updating " << p2next->getI() << " row" << flush;
						_F.init (z, x*y)	;
						_F.addin(z, p2next->getX());
						if (z==0) {
							//cout << ".....deleting \n" << flush;
							p2next = deleteElement(p2next);	
							//if (p2 != NULL) p2 = p2->down;
						} else {
							//cout << ".....new value\n" << flush;
							p2next->setElement(z);
							p2 = p2next; 
							p2next = p2next->up;
						}	  
					} else {
						//add
						//cout << "adding " << p1->getI() << " row\n" << flush;
						_F.init(z, x*y); 
						ijElement<Element> X(p1->getI(), j2, z);
						p2=addElement(p2, X);
					}
				
					p1 = deleteElement(p1); 
				}

                        	if (!Q.empty()) {
					size_t i = Q.front();
					while (rowOcc[i] != 1) {
						Q.pop();
						if (Q.empty()) break;
						i = Q.front();
					}
					if (!Q.empty()) {
						cout << "Adding " << i+1 << " to Q \n" << flush;
			               		reduce(rank, S, mR, mC,os);
						break;
					}
                        	}
                	} else {
				cout << "Elimination of " << S-1<< " rows at rank " <<rank << "\n" << flush;
       
			 	size_t i =0;
		        	bool tworow=false;
				int init_rank = rank;
		        	while  (1) {
		                	while (i < _m) {
						//cout << rowOcc[i] << flush;
						if (rowOcc[i]==0) ++i;
                		        	else if (rowOcc[i]<S)  break;
		                  		else ++i;
			                }
					if (i>=_m) break;
					//cout << "Elimination of row " << i+1 << endl << flush; 
					//cout << "RowGcd=" << rowGcd[i] << "\n" << flush;
					GridElement<Element>* r_p = A[i];
					int min_col = _m+1;
					Element x1;
					int j=-1; 
					vector<pair< size_t, GridElement<Element>*> > j_pts ;
					vector<pair< Element, GridElement<Element>*> > jnext_pts ;
					while (r_p != NULL) {
						//cout << r_p->getJ()+1<< "col," << r_p->getX() << "el" << " ColGcd="<< colGcd[r_p->getJ()] <<"\n" << flush;
						if ((colOcc[r_p->getJ()] < _m+1) && (colOcc[r_p->getJ()] < min_col)) {
							if ((abs(r_p->getX()) == 1/*rowGcd[i]*/) && (abs(r_p->getX())==1/*colGcd[r_p->getJ()]*/)) {
								//cout << "agrees " <<flush; 
								if (j != -1) {
									j_pts.push_back(pair<size_t, GridElement<Element>*>  (j, NULL) );
									jnext_pts.push_back(pair<Element, GridElement<Element>*>  (x1, AT[j]));
								}
                		                        	min_col = colOcc[r_p->getJ()];
                                		        	_F.init(x1, r_p->getX());
		                                        	j = r_p->getJ();
							} else {
								j_pts.push_back( pair<size_t, GridElement<Element>*>  (r_p->getJ(), NULL));
								jnext_pts.push_back( pair<Element, GridElement<Element>*>  (r_p->getX(), AT[r_p->getJ()]));	
							}
						} else {
                                                	j_pts.push_back(pair<size_t, GridElement<Element>*>  (r_p->getJ(), NULL));
                                                        jnext_pts.push_back(pair<Element, GridElement<Element>*>  (r_p->getX(), AT[r_p->getJ()]));
                                                }

						r_p = r_p->next;
					}
		
					j_pts.resize(rowOcc[i]-1);
					jnext_pts.resize(rowOcc[i]-1);
					if (j < 0) {
						//cout << " not applicable" << flush; 
						++i; continue; 
					}
					//else {cout << " Elimination of column " << j+1 << endl << flush;}

		                	if ((abs(x1) == 1/*rowGcd[i]*/) && (abs(x1)==1/*colGcd[j]*/)) {
                		        	if (abs(x1) > 1) cout << "adds " << x1 << "to the diagonal\n"<<flush;
                        			//cout << "Eliminating row "<< i+1 << " by column "<< j+1 << "\n";
		                        	//cout << "Element x" << x << "\n" << flush;
		
				                mR[i] = 1;
                        			mC[j] = 1;
		                        	++rank;

                		                GridElement<Element>* p1=AT[j];

                                		while (p1 != NULL) {
							typename vector<pair< size_t, GridElement<Element>*> >::iterator p2 = j_pts.begin();
							typename vector<pair< Element, GridElement<Element>*> >::iterator p2next = jnext_pts.begin();
							for (; p2 != j_pts.end(); ++p2, ++p2next) {
								//cout << "eliminating column " << p2->first << "\n" << flush;
								Element x; _F.init(x, -(p2next->first)/x1);
								//GridElement<Element>* p2 = j_pts[k].second;
								//GridElement<Element>* p2next = jnext_pts[k].second;
							
		                                        	while (p2next->second != NULL) {
                		                                	if (p2next->second->getI() >= p1->getI()) break;
                                		                	p2->second = p2next->second;
                                               				p2next->second = p2next->second->up;
		                                        	}
                		                        	Element y; _F.init(y, p1->getX());
                                		        	Element z;
                                        			if ((p2next->second!= NULL) && (p2next->second->getI()==p1->getI()) ) {
		                                                	//update
                		                                	//cout << "updating " << p2next->second->getI() << " row" << flush;
                                		                	_F.init (z, x*y)        ;
                                                			_F.addin(z, p2next->second->getX());
		                                                	if (z==0) {
                		                                        	//cout << ".....deleting \n" << flush;
                                		                        	p2next->second = deleteElement(p2next->second);
                                                		        	//if (p2->second != NULL) p2->second = p2->second->down;
		                                                	} else {		
                		                                        	//cout << ".....new value\n" << flush;
                                		                        	p2next->second->setElement(z);
                                                		        	p2->second = p2next->second;
                                                        			p2next->second = p2next->second->up;
		                                                	}
                		                        	} else {
                                		                	//add
                                                			//cout << "adding " << p1->getI() << " row\n" << flush;
		                                                	_F.init(z, x*y);
                		                                	ijElement<Element> X(p1->getI(), p2->first, z);
                                		                	p2->second=addElement(p2->second, X);
                                        			}
							}
                		                 	p1 = deleteElement(p1);
						}
		                	}
					++i;
					if (i%20000==0) cout << "at row " << i << " rank " << rank << "\n" << flush;
		                	if (!Q.empty()) {
                                        	size_t k = Q.front();
                                        	while (rowOcc[k] != 1) {
                                                	Q.pop();
                                                	if (Q.empty()) break;
                                                	k = Q.front();
                                        	}
                                        	if (!Q.empty()) {
							cout << "breaking at " << i << " row\n" << flush;
                		        	//cout << "2 row found at " << i2+1 <<"\n";
                        				break;
						}
		                	}
                			//++i;
        			}

		        	if ((!Q.empty()) || (rank - init_rank > 100)) {
                			reduce(rank, S, mR, mC,os);
		        	}
				break;
			}
        	}		
		return rank;
	}
	
	void write (ostream& out) {
		size_t Omega =0;
		out << _n << " " << _m << " M\n";
		vector<int>::iterator occ_iter = colOcc.begin();
		int j =0;
		for (;occ_iter != colOcc.end(); ++occ_iter,++j) {
			if (*occ_iter > 0) {
				*occ_iter = 0;
				GridElement<Element>* tmp = AT[j];
				GridElement<Element>* tmp2;
				while (tmp != NULL) {
					++Omega;
					out << tmp->getJ()+1 <<" " << tmp->getI()+1 << " " << tmp->getX() << "\n";
					tmp2 = tmp->up;
					delete tmp;
					tmp = tmp2;
				}
			}
		}
		out << "0 0 0\n";	
		cout << "Omega: " << Omega << "\n" << flush;
                rowOcc.clear();
                colOcc.clear();
                //rowGcd.clear();
                //colGcd.clear();
                A.clear();
                AT.clear();
 
	}

	~Grid() {

                vector<int>::iterator occ_iter = colOcc.begin();
                int j =0;
                for (;occ_iter != colOcc.end(); ++occ_iter,++j) {
                        if (*occ_iter > 0) {
                                GridElement<Element>* tmp = AT[j];
				GridElement<Element>* tmp2;
                                while (tmp != NULL) {
					tmp2 = tmp->up;
					delete tmp;
					tmp = tmp2;
                                }
                        }
                }

		rowOcc.clear();
		colOcc.clear();
		//rowGcd.clear();
		//colGcd.clear();
		A.clear();
		AT.clear();		

	}
};

}

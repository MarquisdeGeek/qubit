#ifndef QUBIT_H
#define QUBIT_H

/*
** QuBit - Quantum Superposition Library
** Copyright by S. Goodwin 2001
**
** Freely Distributable under the GPL v2.0
**
** Thanks to Damien Conway and Alan Troth
*/


#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

#define QUBIT_RAND_MAX RAND_MAX


template <typename _T>
class CQuBit { 

	private:
		/*
		** Operator Callback Prototypes
		*/
		typedef _T   (*cbOperation)			(const _T &a, const _T &b);
		typedef _T   (*cbUnaryOperation)	(const _T &a);
		typedef _T   (*cbIncDecOperation)	(const _T &a);
		typedef bool (*cbCondOperation)		(const _T &a, const _T &b);
		/* cbIntOperation's are used when only a 'int' RHS makes sense, ie bit shifting*/
		typedef _T   (*cbIntOperation)		(const _T &a, const int &i);	

		typedef enum { eConj, eDisj, eCollapsedResult, } tQuSuper;

	public:

		CQuBit<_T>() { m_eType = eConj; m_bResult = false; }
		CQuBit<_T>(_T a, _T b, float s=1)		// Construct a range
				{ 
				m_eType = eConj; m_bResult = false; 
				AddRange(a,b,s);
				}

		/*
		** Quantum States
		*/
		void		Clear(void)		{ m_qList.clear(); }
		bool		Add(_T iNewItem)
					{
					typename vector<_T>::const_iterator it;

						/* add if unique */
						for(it=m_qList.begin();it!=m_qList.end();++it)
							if (*it == iNewItem)
								return false;
						
						m_qList.push_back(iNewItem);
						return true;
					}
		bool		AddRange(_T iFirst, _T iLast, float iStep=1)
					{
					bool rt=true;
					float fPosRes = (float)(iLast-iFirst);
						
						fPosRes /= iStep;
						if (fPosRes < 0) fPosRes=-fPosRes;

						Reserve((size_t)fPosRes);
						for(_T i=iFirst;i<=iLast;i+=(_T)iStep)
							rt &= Add(i);
						return rt;
					}
		bool		Remove(_T iOldItem)
					{
					typename vector<_T>::iterator it;

						for(it=m_qList.begin();it!=m_qList.end();++it)
							if (*it == iOldItem)
								{
								m_qList.erase(it);
								return true;
								}
						
						return false;
					}

inline size_t		GetCount(void) const { return m_qList.size(); }
inline _T			GetItem(size_t idx) const 
					{ 
					typename vector<_T>::const_iterator it;

					for(it=m_qList.begin();it!=m_qList.end();++it,idx--)
						if (idx==0)	return *it;
					
					return (_T)0; 
					}

		/*
		** Quantum Operations
		*/
		_T			Any(size_t iUnused)			/* without the 'unused' variable, C++ can not */
					{							/* resolve this overloaded 'Any' with the next*/
					if (m_qList.size() == 0) { return (_T)0; }
					size_t i = (rand()*(m_qList.size()-1))/QUBIT_RAND_MAX;
					return GetItem(i);
					iUnused=iUnused;
					}
		CQuBit<_T>	Any(void)
					{
					CQuBit<_T> any = *this;

						any.SetType(eDisj);
						return any;
					}
		CQuBit<_T>	Any(CQuBit<_T> &a) { return Any(*this, a); }			/*union*/
		CQuBit<_T>	Any(CQuBit<_T> &a, CQuBit<_T> &b)	/*union*/
					{
					CQuBit<_T> ans;

						if (a.GetType() != eCollapsedResult)
							{
							typename vector<_T>::iterator it;

							for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
								ans.Add(*it);
							}

						if (b.GetType() != eCollapsedResult)
							{
							typename vector<_T>::iterator it;

							for(it=b.m_qList.begin();it!=b.m_qList.end();++it)
								ans.Add(*it);
							}

						ans.SetType(eDisj);
						return ans;
					}
		CQuBit<_T>	All(void) 
					{
					CQuBit<_T> all = *this;

						all.SetType(eConj);
						return all;
					}
		CQuBit<_T>	All(CQuBit<_T> &a) { return All(*this, a); }					/*intersection*/
		CQuBit<_T>	All(CQuBit<_T> &a, CQuBit<_T> &b)	/*intersection*/
					{
					CQuBit<_T> ans;

						if (a.GetType() != eCollapsedResult && 
							b.GetType() != eCollapsedResult)
							{
							typename vector<_T>::iterator ita, itb;

							for(ita=a.m_qList.begin();ita!=a.m_qList.end();ita++)
								for(itb=b.m_qList.begin();itb!=b.m_qList.end();itb++)
									if (*ita == *itb)
										ans.Add(*ita);
							}
						else if (a.GetType() == eCollapsedResult)
							{
							ans = b;
							}
						else if (b.GetType() == eCollapsedResult)
							{
							ans = a;
							}

						ans.SetType(eConj);
						return ans;
					}

		CQuBit<_T>	Eigenstates(void)
					{
					CQuBit<_T> e;

						if (GetType() == eCollapsedResult)
							{
							typename vector<_T>::const_iterator it;

							for(it=m_Eigenstates.begin();it!=m_Eigenstates.end();++it)
								e.Add(*it);
							
							e.SetType(m_eEigenType);
							}

						return e;
					}
	inline tQuSuper GetType(void) const			{ return m_eType; }
	inline	bool	GetBoolResult(void) const	{ return m_bResult; }
		
		/*
		** Overloads
		*/
		CQuBit<_T> operator!(void) const 
		{	CQuBit<_T> ans;	
			ans.do_unary_oper(*this, &CQuBit<_T>::qop_not);	
			return ans;	}
		CQuBit<_T> operator~(void) const
		{	CQuBit<_T> ans;	
			ans.do_unary_oper(*this, &CQuBit<_T>::qop_one);	
			return ans;	}
		CQuBit<_T> operator-(void) const
		{	CQuBit<_T> ans;	
			ans.do_unary_oper(*this, &CQuBit<_T>::qop_neg);	
			return ans;	}

		CQuBit<_T> operator%(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_mod);	
			return ans;	}
		CQuBit<_T> operator%(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_mod);	
			return ans;	}
		CQuBit<_T> &operator%=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_mod);	
			return *this=ans;	}
		CQuBit<_T> &operator%=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_mod);	
			return *this=ans;	}

		CQuBit<_T> operator&(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_band);	
			return ans;	}
		CQuBit<_T> operator&(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_band);	
			return ans;	}
		CQuBit<_T> &operator&=(const CQuBit<_T> &rhs)
		{	CQuBit ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_band);	
			return *this=ans;	}
		CQuBit<_T> &operator&=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_band);	
			return *this=ans;	}

		CQuBit<_T> operator&&(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_land);	
			return ans;	}
		CQuBit<_T> operator&&(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_land);	
			return ans;	}

		CQuBit<_T> operator*(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_mul);			
			return ans;	}
		CQuBit<_T> operator*(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_mul);	
			return ans;	}
		CQuBit<_T> &operator*=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_mul);			
			return *this=ans;	}
		CQuBit<_T> &operator*=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_mul);	
			return *this=ans;	}

		CQuBit<_T> operator+(CQuBit<_T> const &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_add);			
			return ans;	}
		CQuBit<_T> operator+(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_add);	
			return ans;	}
		CQuBit<_T> &operator+=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_add);		
			return *this=ans;	}
		CQuBit<_T> &operator+=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_add);	
			return *this=ans;	}

		CQuBit<_T> operator-(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_sub);			
			return ans;	}
		CQuBit<_T> operator-(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_sub);	
			return ans;	}
		CQuBit<_T> &operator-=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_sub);			
			return *this=ans;	}
		CQuBit<_T> &operator-=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_sub);	
			return *this=ans;	}

		CQuBit<_T> operator/(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_div);			
			return ans;	}
		CQuBit<_T> operator/(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_div);	
			return ans;	}
		CQuBit<_T> &operator/=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_div);			
			return *this=ans;	}
		CQuBit<_T> &operator/=(const _T &rhs)		
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_div);	
			return *this=ans;	}


		/* These operations only make sense with RHSintegrals (and will only 
		   generally be used with CQuBit<int> */
		CQuBit<_T> operator<<(const int &rhs) const		
		{	CQuBit<_T> ans;	
			ans.do_oper_int(*this, rhs, &CQuBit<_T>::qop_shlt);	
			return ans;	}
		CQuBit<_T> &operator<<=(const int &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_int(*this, rhs, &CQuBit<_T>::qop_shlt);	
			return *this=ans;	}
		CQuBit<_T> operator>>(const int &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_int(*this, rhs, &CQuBit<_T>::qop_shrt);	
			return ans;	}
		CQuBit<_T> &operator>>=(const int &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_int(*this, rhs, &CQuBit<_T>::qop_shrt);	
			return *this=ans;	
		}
		
		CQuBit<_T> &operator=(const CQuBit<_T> &q)
		{
			if (this != &q)
				{
				typename vector<_T>::const_iterator it;

				m_qList.clear();
				for(it=q.m_qList.begin();it!=q.m_qList.end();++it)
					Add(*it);

				m_Eigenstates.clear();
				for(it=q.m_Eigenstates.begin();it!=q.m_Eigenstates.end();++it)
					m_Eigenstates.push_back(*it);
			
				m_bResult = q.m_bResult;
				m_eType = q.m_eType;
				m_eEigenType = q.m_eEigenType;
				}
			return *this;
		}

		CQuBit<_T> operator^(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_xor);			
			return ans;	}
		CQuBit<_T> operator^(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_xor);	
			return ans;	}
		CQuBit<_T> &operator^=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_xor);			
			return *this=ans;	}
		CQuBit<_T> &operator^=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_xor);	
			return *this=ans;	}

		CQuBit<_T> operator|(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_bor);			
			return ans;	}
		CQuBit<_T> operator|(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_bor);	
			return ans;	}
		CQuBit<_T> &operator|=(const CQuBit<_T> &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_bor);			
			return *this=ans;	}
		CQuBit<_T> &operator|=(const _T &rhs)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_bor);	
			return *this=ans;	}

		CQuBit<_T> operator||(const CQuBit<_T> &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper(*this, rhs, &CQuBit<_T>::qop_lor);			
			return ans;	}
		CQuBit<_T> operator||(const _T &rhs) const
		{	CQuBit<_T> ans;	
			ans.do_oper_type(*this, rhs, &CQuBit<_T>::qop_lor);	
			return ans;	}

		CQuBit<_T> &operator++(void)			/* prefix */
		{	
		CQuBit c=*this;

			m_qList.clear();
			do_incdec_oper(c, qop_inc);
			return *this;
		}
		CQuBit<_T> operator++(int)				/* postfix */
		{	
		CQuBit c=*this;

			m_qList.clear();
			do_incdec_oper(c, qop_inc);
			return c;
		}
		CQuBit<_T> &operator--(void)			/* prefix */
		{	
		CQuBit c=*this;

			m_qList.clear();
			do_incdec_oper(c, qop_dec);
			return *this;
		}
		CQuBit<_T> operator--(int)				/* postfix */
		{	
		CQuBit c=*this;

			m_qList.clear();
			do_incdec_oper(c, qop_dec);
			return c;
		}

		CQuBit<_T> operator<=(const CQuBit<_T> &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_lte); 
			return ans; }
		CQuBit<_T> operator<=(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_lte); 
			return ans; }
		CQuBit<_T> operator<(const CQuBit<_T> &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_lt); 
			return ans; }
		CQuBit<_T> operator<(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_lt); 
			return ans; }
		CQuBit<_T> operator>=(const CQuBit<_T> &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_gte); 
			return ans; }
		CQuBit<_T> operator>=(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_gte); 
			return ans; }
		CQuBit<_T> operator>(const CQuBit<_T> &rhs) const
			{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_gt); 
			return ans; }
		CQuBit<_T> operator>(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_gt); 
			return ans; }
		CQuBit<_T> operator==(const CQuBit<_T> &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_eq); 
			return ans; }
		CQuBit<_T> operator==(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_eq); 
			return ans; }
		CQuBit<_T> operator!=(const CQuBit<_T> &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition(*this, rhs, &CQuBit<_T>::qco_neq); 
			return ans; }
		CQuBit<_T> operator!=(const _T &rhs) const
		{ CQuBit<_T> ans; 
			ans.do_condition_type(*this, rhs, &CQuBit<_T>::qco_neq); 
			return ans; }

	/*	operator bool() const */
	friend  CQuBit<_T> operator%(const _T &a, const CQuBit<_T> &b)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(a, b, &CQuBit<_T>::qop_mod);	
			return ans;	}
	friend  CQuBit<_T> operator/(const _T &a, const CQuBit<_T> &b)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(a, b, &CQuBit<_T>::qop_div);	
			return ans;	}
#if 0
	friend  CQuBit<_T> operator+(const _T &a, const CQuBit<_T> &b)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(a, b, &CQuBit<_T>::qop_add);	
			return ans;	}
	friend  CQuBit<_T> operator*(const _T &a, const CQuBit<_T> &b)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(a, b, &CQuBit<_T>::qop_mul);	
			return ans;	}
	friend  CQuBit<_T> operator-(const _T &a, const CQuBit<_T> &b)
		{	CQuBit<_T> ans;	
			ans.do_oper_type(a, b, &CQuBit<_T>::qop_sub);	
			return ans;	}
#endif 
		/*
		** User-Retrieval
		*/ 
	const _T operator[](size_t idx) const { return GetItem(idx); }

		/*
		** Function Operators
		*/
			CQuBit<_T>	Floor(void) const		/* integise qubit */
				{
				CQuBit<_T> ans;
				ans.do_unary_oper(*this, &CQuBit<_T>::qop_floor);			
				return ans;	}

		/*
		** Stream Handling
		*/
	friend  ostream &operator<<(ostream &os, const CQuBit<_T> &q)
				{
				typename vector<_T>::const_iterator it;

					os << "{ ";
					for(it=q.m_qList.begin();it!=q.m_qList.end();++it)
						os << *it << " ";
					os << "}";
					return os;
				}
	friend  istream &operator>>(istream &is, CQuBit<_T> &q)
				{
				_T	val;
				char c;

					do
						is >> c;
					while(!is.eof() && c!='{');
					
					do
						{
						do 
							is >> c;
						while(!is.eof() && c!='}' && !isdigit(c));
						
						if (isdigit(c))
							{
							is.putback(c); 
							is >> val;
							q.Add(val);
							}					
						}
					while(!is.eof() && c!='}');
					
					return is;
				}

	private:
		/*
		** Implementation
		*/
		bool						m_bResult;		/* from a condition */
		tQuSuper					m_eType;
		tQuSuper					m_eEigenType;
		vector<_T, allocator<_T> >	m_qList;
		vector<_T, allocator<_T> >	m_Eigenstates;

	inline	void	SetType(tQuSuper t) { m_eType = t; }
	inline	void	Reserve(size_t i)	{ m_qList.reserve(i); }
		
		/*
		** Operator Handling
		*/
		bool	do_oper			(const CQuBit<_T> &a, const CQuBit<_T> &b, cbOperation cb)
				{
				typename vector<_T>::iterator ita, itb;

					if (a.GetType() == eCollapsedResult)	return false;
					if (b.GetType() == eCollapsedResult)	return false;
					
					Reserve(a.GetCount()*b.GetCount());
					for(ita=a.m_qList.begin();ita!=a.m_qList.end();ita++)
						for(itb=b.m_qList.begin();itb!=b.m_qList.end();itb++)
							Add(cb(*ita, *itb));
					
					SetType(a.GetType());
					
					return true;
				}

		bool	do_oper_type	(const CQuBit<_T> &a, const _T &b, cbOperation cb)
				{
				typename vector<_T>::const_iterator it;

					if (a.GetType() == eCollapsedResult)	return false;
					
					Reserve(a.GetCount());

					for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
						Add(cb(*it, b));
					
					SetType(a.GetType());
					
					return true;
				}
		bool	do_oper_type	(const _T &a, const CQuBit<_T> &b, cbOperation cb)
				{
				typename vector<_T>::const_iterator it;
					if (b.GetType() == eCollapsedResult)	return false;
					
					Reserve(b.GetCount());

					for(it=b.m_qList.begin();it!=b.m_qList.end();++it)
						Add(cb(a, *it));
					
					SetType(b.GetType());
					
					return true;
				}
		bool	do_unary_oper	(const CQuBit<_T> &a, cbUnaryOperation cb)
				{
				typename vector<_T>::const_iterator it;

					if (a.GetType() == eCollapsedResult)	return false;
					
					Reserve(a.GetCount());

					for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
						Add(cb(*it));
					
					SetType(a.GetType());
					
					return true;
				}

		bool	do_incdec_oper	(const CQuBit<_T> &a, cbIncDecOperation cb)
				{
				typename vector<_T>::const_iterator it;

					if (a.GetType() == eCollapsedResult)	return false;
					
					Reserve(a.GetCount());

					for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
						Add(cb(*it));
					
					SetType(a.GetType());
					
					return true;
				}

		bool	do_condition	(const CQuBit<_T> &a, const CQuBit<_T> &b, cbCondOperation cb)
				{
				typename vector<_T>::const_iterator ita, itb;
				bool rt, conj, disj;

					/* If both have collapsed, compare as if they were booleans, otherwise*/
					if (a.GetType() == eCollapsedResult && b.GetType() == eCollapsedResult)
						return cb(a.GetBoolResult(), b.GetBoolResult());
					if (a.GetType() == eCollapsedResult || b.GetType() == eCollapsedResult)
						return false;
					
					Reserve(a.GetCount());

					/* Note: Not optimal (since we could early out upon failure), but
					   I feel it demonstrates the workings better.*/

					m_Eigenstates.clear();

					for(ita=a.m_qList.begin();ita!=a.m_qList.end();ita++)
						{
						conj = true;
						disj = false;
						
						for(itb=b.m_qList.begin();itb!=b.m_qList.end();itb++)
							{
							rt = cb(*ita, *itb);
							conj &= rt;
							disj |= rt;
							}
						
						if (b.GetType() == eConj && conj)
							m_Eigenstates.push_back(*ita);

						if (b.GetType() == eDisj && disj)
							m_Eigenstates.push_back(*ita);
						}
					
					SetType(eCollapsedResult);
					m_eEigenType = a.GetType();
					
					m_bResult = false;
					if (a.GetType() == eConj && m_Eigenstates.size()==a.GetCount())
						m_bResult = true;
					if (a.GetType() == eDisj && m_Eigenstates.size())
						m_bResult = true;
					
					return true;
				}

		bool	do_condition_type(const CQuBit<_T> &a, const _T &b, cbCondOperation cb)
				{
				typename vector<_T>::const_iterator it;
				bool rt, conj, disj;

					if (a.GetType() == eCollapsedResult)
						return a.GetBoolResult();
					
					Reserve(a.GetCount());
					m_Eigenstates.clear();

					conj = true;
					disj = false;
					for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
						{
						rt = cb(*it, b);
						if (rt) {
							m_Eigenstates.push_back(*it);
						}
						conj &= rt;
						disj |= rt;
						}
					
					SetType(eCollapsedResult);
					m_eEigenType = a.GetType();
					
					m_bResult = false;
					
					if (a.GetType() == eConj && conj)
						m_bResult = true;
					if (a.GetType() == eDisj && disj)
						m_bResult = true;
					
					return true;
				}
		bool	do_oper_int		(const CQuBit<_T> &a, const int &b, cbIntOperation cb)
				{
				typename vector<_T>::const_iterator it;

					for(it=a.m_qList.begin();it!=a.m_qList.end();++it)
						Add(cb(*it, b));
					
					SetType(a.GetType());
					
					return true;
				}

		
		/*
		** Operator Callback Functions
		*/
		/* unary */
		static _T qop_not(const _T &a) {	return !a; }
		static _T qop_one(const _T &a) {	return ~a; }
		static _T qop_neg(const _T &a) {	return -a; }

		/* binary */
		static _T qop_neq(const _T &a, const _T &b) {	return a!=b; }
		static _T qop_band(const _T &a, const _T &b) {	return a&b; }
		static _T qop_bor(const _T &a, const _T &b) {	return a|b; }
		static _T qop_land(const _T &a, const _T &b) {	return a&&b; }
		static _T qop_lor(const _T &a, const _T &b) {	return a||b; }
		/* binary (arithmetic) */
		static _T qop_add(const _T &a, const _T &b) {	return a+b; }
		static _T qop_sub(const _T &a, const _T &b) {	return a-b; }
		static _T qop_mul(const _T &a, const _T &b) {	return a*b; }
		static _T qop_div(const _T &a, const _T &b) {	return a/b; }
		static _T qop_mod(const _T &a, const _T &b) {	return a%b; }
		static _T qop_xor(const _T &a, const _T &b) {	return a^b; }
		/* binary (shift) */
		static _T qop_shlt(const _T &a, const _T &i) {	return a<<i; }
		static _T qop_shrt(const _T &a, const _T &i) {	return a>>i; }
		/* binary (inc/dev)
		   In both cases here we are concerned about the 'value' of the
		   variable - not a reference to it, as C++ would dictate for
		   prefix increments, i.e. ++a or --b
		*/
		static _T qop_inc(const _T &a) { _T b=a; b++; return b; }	
		static _T qop_dec(const _T &a) { _T b=a; b--; return b; }
		/* binary (comparisons) */
		static bool qco_lte(const _T &a, const _T &b) { return a<=b; }
		static bool qco_lt(const _T &a, const _T &b) { return a<b; }
		static bool qco_gte(const _T &a, const _T &b) { return a>=b; }
		static bool qco_gt(const _T &a, const _T &b) { return a>b; }
		static bool qco_eq(const _T &a, const _T &b) { return a==b; }
		static bool qco_neq(const _T &a, const _T &b) { return a!=b; }
		/* special */
		static _T qop_floor(const _T &a) { return (_T)(int)a; }

	};


#endif	

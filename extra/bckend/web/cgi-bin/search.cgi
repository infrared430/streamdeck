#!/usr/bin/python3
# THIS IS FULL OF SQL INJECTION POINTS
#select * from recipe where regexp_split_to_array(lower(title), '\s+') @> array['arroz'] or regexp_split_to_array(lower(description),'\s+') @> array['arroz']
import os
from urllib import parse
import psycopg2

query_str=os.environ['QUERY_STRING']
query=parse.parse_qs(query_str)
keywords=query['q'][0].lower().split() if 'q' in query else None
clasf=query.get('t',['t'])[0].lower() if 't' in query else None
token=query.get('k',['k'])[0].lower() if 'k' in query else None
locale=query.get('l',['l'])[0] if 'l' in query else None

def getConn():
    conn = psycopg2.connect(host='localhost',database='cook',user='cook',password='cp cook v0')
    return conn

# export QUERY_STRING="t=t&l=13&q=mantequilla+crema"
def findCook(keywords,locale_id):
    conn = None
    out=""
    try:
        conn=getConn()
        cur=conn.cursor();
        query="WITH RECURSIVE locale_recursive AS ( SELECT id as base, id, lang,zone,fine, master FROM public.locale UNION ALL SELECT cr.base, c.id, c.lang, c.zone, c.fine, c.master FROM public.locale c INNER JOIN locale_recursive cr ON c.id = cr.master ), locale_resolver AS ( SELECT row_number() over (order by id desc) rn, * FROM locale_recursive ), token_resolver as ( select * from localized i18n left join locale_resolver lr on i18n.locale_id=lr.id ), recipe_resolver as ( select r.id, tr_t.value as title_,tr_d.value as description_, r.author as title from recipe r join token_resolver tr_t on r.token_title=tr_t.token and tr_t.base=%s join token_resolver tr_d on r.token_description=tr_d.token and tr_d.base=%s order by tr_t.rn asc, tr_d.rn asc fetch first 1 row only ) select r.id, r.title_,r.description_ from recipe_resolver r where regexp_split_to_array(lower(r.title_),'\s+') @> %s or regexp_split_to_array(lower(r.description_),'\s+') @> %s",[locale_id, locale_id, keywords, keywords]
        cur.execute(*query)
        results=cur.fetchall()
        out+=""
        for res in results:
            pl=str(res[0])+";"+str(res[1])
            out+=str(len(pl.encode('utf-8')))+"\n"
            out+=pl+"\n"
        out+="-1\n"
        cur.close()
        return out
    except (Exception, psycopg2.DatabaseError) as error:
        out=str(error)
        return out
    finally:
        if conn is not None:
            conn.close()

# export QUERY_STRING="t=r&q=3"
def findRecipe(recId,localeid):
    conn = None
    out=""
    try:
        conn= getConn()
        cur=conn.cursor()
        query="""
WITH RECURSIVE locale_recursive AS ( 
    SELECT id as base, id, lang,zone,fine, master 
      FROM public.locale 
 UNION ALL 
    SELECT cr.base, c.id, c.lang, c.zone, c.fine, c.master 
      FROM public.locale c 
INNER JOIN locale_recursive cr ON c.id = cr.master 
), 
p_locale_id AS (
    select %s AS locale_id
),
locale_resolver AS ( 
    SELECT row_number() over (order by id desc) rn, * 
      FROM locale_recursive 
     where base=(select locale_id from p_locale_id)
), 
token_resolver as ( 
    select * from localized i18n 
 left join locale_resolver lr on i18n.locale_id=lr.id 
) 
select  
       coalesce(s.steps,0) as steps, 
       tr_t.value as title_,
       tr_d.value as description_, 
       coalesce(v.alias,v.name,v.email,cast(v.id as varchar),'unknown') as author 
  from recipe r 
left join (select s_.recipe_id, count(1) as steps from step s_ group by s_.recipe_id) s on s.recipe_id=r.id
     join token_resolver tr_t on r.token_title=tr_t.token join token_resolver tr_d on r.token_description=tr_d.token
     join viewer v on v.id=r.author
 where r.id=%s 
order by tr_t.rn asc, tr_d.rn asc fetch first 1 row only
        """,[localeid,recId]
        cur.execute(*query)
        results=cur.fetchone()
        if(results != None):
            # step count
            out+=str(len(str(results[0])))+"\n"
            out+=str(results[0])+"\n"
            # title literal
            out+=str(len(str(results[1])))+"\n"
            out+=str(results[1])+"\n"
            # description literal
            out+=str(len(results[2].encode('utf-8')))+"\n"
            out+=results[2]+"\n"
            #author literal
            out+=str(len(results[3].encode('utf-8')))+"\n"
            out+=results[3]+"\n"
        out+="-1\n"
        return out
    except (Exception, psycopg2.DatabaseError) as error:
        out=str(error)
        return out
    finally:
        if conn is not None:
            conn.close()
# t=s&q=1+1&l=3
def getStep(recId, stepId):
    step_select="select coalesce(wait_time,0) wait_time, coalesce(condition,'') condition, token_description, coalesce(token_wait,'') token_wait from step where recipe_id=%s and seq=%s",[recId,stepId]
    conn=None
    out=""
    try:
        conn=getConn()
        cur=conn.cursor()
        cur.execute(*step_select)
        results=cur.fetchone()
        if(results != None):
            out+=str(len(str(results[0])))+"\n"
            out+=str(results[0])+"\n" # wait_time
            out+=str(len(results[1].encode('utf-8')))+"\n"
            out+=results[1]+"\n" # condition
            out+=str(len(results[2].encode('utf-8')))+"\n"
            out+=results[2]+"\n" # token_description
            out+=str(len(results[3].encode('utf-8')))+"\n"
            out+=results[3]+"\n" # token wait
        out+="-1\n"
        return out
    except (Exception, psycopg2.DatabaseError) as error:
        out=str(error)
        return out
    finally:
        if conn is not None:
            conn.close()

def findToken(token, locale_id):
    token_select="with recursive locale_recursive as (select id as base, id, lang, zone, fine, master from locale union all select cr.base, c.id, c.lang, c.zone, c.fine, c.master from public.locale c inner join locale_recursive cr on c.id=cr.master), locale_resolver as (select row_number() over (order by id desc), * from locale_recursive) select i18n.value from localized i18n left join locale_resolver lr on i18n.locale_id=lr.id where token=%s and lr.base=%s order by lr.id desc fetch first row only",[token,locale_id]
    conn=None
    out=""
    try:
        conn=getConn()
        cur=conn.cursor()
        cur.execute(*token_select)
        results=cur.fetchone()
        out+=str(len(results[0].encode('utf-8')))+"\n"
        out+=results[0]+"\n"
        out+="-1\n"
        return out
    except (Exception, psycopg2.DatabaseError) as error:
        out=str(error)
        return out
    finally:
        if conn is not None:
            conn.close()

out=""
# titles
if clasf == 't':
    out=findCook(keywords,int(locale))
#recipe
if clasf == 'r':
    out=findRecipe(keywords[0],int(locale))
if clasf == 'k':
    out=findToken(token,int(locale))
if clasf == 's':
    out=getStep(int(keywords[0]),int(keywords[1]))
#headers
print("Content-Type: text/plain;charset=utf-8");
print("Content-Length: "+str(len(out.encode('utf-8'))))
print("");
print(out);

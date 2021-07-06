DROP VIEW IF EXISTS user_logged_actions;
DROP FUNCTION IF EXISTS public.audit_logged_users_function();
DROP TABLE IF EXISTS audit_logged_users_table;

CREATE TABLE public.audit_logged_users_table
(
    logged_user text COLLATE pg_catalog."default",
    login_start_time timestamp without time zone,
    database_name text COLLATE pg_catalog."default",
    process_id integer NOT NULL,
    connection_status text COLLATE pg_catalog."default",
    session_type text COLLATE pg_catalog."default",
	ip_address text,
	session_hostname text,
	current_query text,
	cur_qry_exec_time timestamp without time zone,
	last_query text,
	lst_qry_exec_time timestamp without time zone,
    CONSTRAINT audit_logged_users_table_pkey PRIMARY KEY (process_id)
);



CREATE OR REPLACE FUNCTION public.audit_logged_users_function(
	)
    RETURNS TABLE(logged_usr text, login_starttime timestamp without time zone, db_name text, proc_id integer, conn_status text, ses_type text,ip_adr text,sess_hostname text,
				  curr_query text,curr_qry_extime timestamp without time zone, last_query text,last_qry_extime timestamp without time zone) 
    LANGUAGE 'plpgsql'

    COST 100
    VOLATILE 
    ROWS 1000
AS $BODY$

DECLARE 
	cur_proc_id integer;
	row_data record;
	var_pid_rec record;
	change_pid_data_rec record;
BEGIN
	SELECT pg_backend_pid() INTO cur_proc_id;
	--remove current_database to get all database information 
		FOR row_data IN (SELECT datname,usename,pid,backend_start,state,client_addr,client_hostname,query,query_start FROM pg_stat_activity 
						 where datname IS NOT NULL AND pid NOT IN (SELECT process_id FROM audit_logged_users_table)
                     AND pid <> cur_proc_id) LOOP
		INSERT INTO audit_logged_users_table(logged_user,login_start_time,database_name,process_id,connection_status,session_type,ip_address,session_hostname,current_query,cur_qry_exec_time)
		VALUES (row_data.usename,row_data.backend_start,row_data.datname,row_data.pid,row_data.state,'User Session',row_data.client_addr,row_data.client_hostname,row_data.query,row_data.query_start);
					 
		END LOOP;
	
	UPDATE audit_logged_users_table SET connection_status='active',cur_qry_exec_time=(SELECT query_start FROM pg_stat_activity WHERE pid=cur_proc_id)
	WHERE process_id=cur_proc_id;
	IF ((SELECT count(*) FROM audit_logged_users_table alut WHERE alut.process_id=cur_proc_id)=0) THEN
	    FOR row_data IN (SELECT datname,usename,pid,backend_start,state,client_addr,client_hostname,query,query_start from pg_stat_activity where pid=cur_proc_id) LOOP
			INSERT INTO audit_logged_users_table(logged_user,login_start_time,database_name,process_id,connection_status,session_type,ip_address,session_hostname,current_query,cur_qry_exec_time)
			VALUES (row_data.usename,row_data.backend_start,row_data.datname,row_data.pid,row_data.state,'User Session',row_data.client_addr,row_data.client_hostname,row_data.query,row_data.query_start);
		END LOOP;
	END IF;
	
	IF ((SELECT count(*) FROM pg_stat_activity where pid IN (SELECT distinct process_id FROM audit_logged_users_table WHERE process_id NOT IN(SELECT distinct pid FROM pg_stat_activity) AND connection_status <> 'Inactive') )=0) THEN
		UPDATE audit_logged_users_table  SET connection_status='Inactive', last_query=current_query ,current_query=NULL ,lst_qry_exec_time=cur_qry_exec_time,cur_qry_exec_time=NULL
		WHERE process_id NOT IN (SELECT pid FROM pg_stat_activity) AND connection_status <> 'Inactive' ;
	END IF;
	
	FOR var_pid_rec IN 
      (SELECT distinct process_id FROM audit_logged_users_table WHERE process_id <> cur_proc_id AND connection_status<>'Inactive' ) LOOP
	  
		IF ((SELECT state FROM pg_stat_activity WHERE pid IN (SELECT distinct process_id FROM audit_logged_users_table WHERE process_id= var_pid_rec.process_id AND connection_status<>'Inactive'))NOT IN ('active')) THEN
		 FOR change_pid_data_rec IN 
		   (SELECT state,query,query_start FROM pg_stat_activity WHERE pid IN (SELECT DISTINCT process_id FROM audit_logged_users_table WHERE process_id = var_pid_rec.process_id AND connection_status<>'Inactive'))
		 LOOP
		 UPDATE audit_logged_users_table  SET connection_status=change_pid_data_rec.state,current_query=change_pid_data_rec.query,cur_qry_exec_time=change_pid_data_rec.query_start
		   WHERE connection_status <> 'Inactive' AND  process_id IN (SELECT pid FROM pg_stat_activity WHERE 
			pid IN (SELECT DISTINCT process_id FROM audit_logged_users_table WHERE process_id = var_pid_rec.process_id AND  connection_status<>'Inactive') );
		END LOOP;
		END IF;
	END LOOP;
	RETURN QUERY SELECT * FROM audit_logged_users_table;
END;

$BODY$;

CREATE OR REPLACE VIEW user_logged_actions AS SELECT * FROM audit_logged_users_function();



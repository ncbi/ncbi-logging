SET plpgsql.extra_warnings TO 'shadowed_variables';
CREATE OR REPLACE FUNCTION add_session(in_ip text,
                                     in_ipint decimal (39,0),
                                     in_acc text,
                                     in_agent text,
                                     in_domain text,
                                     in_res int,
                                     in_start_ts timestamp,
                                     in_end_ts timestamp,
                                     in_bytes bigint,
                                     in_cnt bigint,
                                     in_source text)
RETURNS VOID AS
$$
    DECLARE
        v_ip_id bigint:=0;
        v_acc_id int:=0;
        v_agent_id int:=0;
        v_domain_id int:=0;
        v_start_ts timestamp;
    BEGIN
        INSERT INTO ips(ip, ipint) VALUES (in_ip, in_ipint)
            ON CONFLICT DO NOTHING;
        SELECT ip_id INTO STRICT v_ip_id FROM ips WHERE in_ip=ip
               --and in_ipint=ipint
            ;

        INSERT INTO accs(acc, acc_part) VALUES (in_acc, in_acc)
            ON CONFLICT DO NOTHING;
        SELECT acc_id INTO STRICT v_acc_id FROM accs where in_acc=acc;

        INSERT INTO agents(agent) VALUES (in_agent)
            ON CONFLICT DO NOTHING;
        SELECT agent_id INTO STRICT v_agent_id FROM agents
            where in_agent=agent;

        INSERT INTO domains(domain) VALUES (in_domain)
            ON CONFLICT DO NOTHING;
        SELECT domain_id INTO STRICT v_domain_id FROM domains
            where in_domain=domain;

        INSERT INTO sources(source) VALUES (in_source)
            ON CONFLICT DO NOTHING;

        SELECT start_ts INTO v_start_ts
        FROM sessions
        WHERE
            ip_id = v_ip_id AND
            acc_id = v_acc_id AND
            agent_id = v_agent_id AND
            res = in_res AND
            source = in_source AND
            (start_ts, end_ts) OVERLAPS
            (in_start_ts - interval '29 minutes',
             in_end_ts   + interval '29 minutes')
        ORDER BY start_ts
        LIMIT 1;


        IF NOT FOUND THEN
            INSERT INTO sessions
            (ip_id, acc_id, agent_id, domain_id, res,
             start_ts, end_ts, bytes, cnt,
             created, updated, source)
            VALUES
            (v_ip_id, v_acc_id, v_agent_id, v_domain_id, in_res,
             in_start_ts, in_end_ts, in_bytes, in_cnt,
             now(), now(), in_source);
        ELSE
            UPDATE sessions
            SET
                cnt=cnt + in_cnt,
                bytes=bytes + in_bytes,
                start_ts=LEAST(start_ts, in_start_ts),
                end_ts=GREATEST(end_ts, in_end_ts),
                updated=now()
            WHERE
                start_ts = v_start_ts AND
                ip_id = v_ip_id AND
                acc_id = v_acc_id AND
                source = in_source AND
                agent_id = v_agent_id;
        END IF;

    END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION add_storage(in_bucket text,
                                     in_start_ts timestamp,
                                     in_bytes bigint,
                                     in_source text)
RETURNS VOID AS
$$
    DECLARE
        v_start_ts timestamp;
    BEGIN
        INSERT INTO sources(source) VALUES (in_source)
            ON CONFLICT DO NOTHING;

        INSERT INTO storage (start_ts, bytes, bucket, source, created, updated)
        VALUES (in_start_ts, in_bytes, in_bucket, in_source, now(), now());
    END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION Dot2LongIP(text) RETURNS bigint AS '
    SELECT
    split_part($1,''.'',1)::int8*(256*256*256)+
    split_part($1,''.'',2)::int8*(256*256)+
    split_part($1,''.'',3)::int8*256+
    split_part($1,''.'',4)::int8;
' LANGUAGE SQL;

/*
select add_sra(
    '1.1.1.1'::text,
    1111::decimal(39,0),
    'acc'::text,
    'agent'::text,
    'domain'::text,
    200,
    localtimestamp,
    localtimestamp + interval '1 second',
    100::bigint,
    1);
*/

DROP FUNCTION IF EXISTS bigint_to_inet(BIGINT);
CREATE FUNCTION bigint_to_inet(BIGINT) RETURNS inet AS $$
SELECT (($1>>24&255)||'.'||($1>>16&255)||'.'||($1>>8&255)||'.'
    ||($1>>0&255))::inet
$$ LANGUAGE SQL strict immutable;
GRANT EXECUTE ON FUNCTION bigint_to_inet(BIGINT) TO public;

DROP FUNCTION IF EXISTS inet_to_bigint(inet);
CREATE OR REPLACE FUNCTION inet_to_bigint(inet) RETURNS BIGINT AS $$
SELECT $1 - inet '0.0.0.0'
$$ LANGUAGE SQL strict immutable;
GRANT EXECUTE ON FUNCTION inet_to_bigint(inet) TO public;

-- select add_session('3'::text, 1::decimal(39,0),
-- '3'::text, '3'::text,
-- 3,'2008-01-01'::timestamp, '2008-01-01'::timestamp,
-- 3::bigint,4,'t'::text);

-- select add_sra(ips.ip, ips.ipint, accs.acc, agents.agent, domains.domain, res, start_ts, end_ts, bytes, cnt) from sra_bak, ips, accs, agents, domains where sra_bak.ip_id=ips.ip_id and sra_bak.acc_id=accs.acc_id and sra_bak.agent_id=agents.agent_id and sra_bak.domain=domains.domain_id;


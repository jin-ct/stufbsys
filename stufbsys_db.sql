--
-- PostgreSQL database dump
--

\restrict sxN143EiG5DehfUmQOAQdDUpzPfGsI1aAtxmLReblfeIIPz4EJJFQLqxIFS10Gj

-- Dumped from database version 17.6
-- Dumped by pg_dump version 17.6

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

ALTER TABLE ONLY public.vote_record DROP CONSTRAINT vote_record_pkey;
ALTER TABLE ONLY public."user" DROP CONSTRAINT user_pkey;
ALTER TABLE ONLY public.teacher_suggestion DROP CONSTRAINT teacher_suggestion_pkey;
ALTER TABLE ONLY public.teacher DROP CONSTRAINT teacher_pkey;
ALTER TABLE ONLY public.teacher_data_statistics DROP CONSTRAINT teacher_data_statistics_pkey;
ALTER TABLE ONLY public.school_suggestion_short DROP CONSTRAINT school_suggestion_short_pkey;
ALTER TABLE ONLY public.school_suggestion_long DROP CONSTRAINT school_suggestion_short_copy1_pkey;
ALTER TABLE ONLY public.platform_settings DROP CONSTRAINT platform_settings_pkey;
ALTER TABLE ONLY public.notice DROP CONSTRAINT notice_pkey;
ALTER TABLE ONLY public.entrance_exam DROP CONSTRAINT entrance_exam_pkey;
ALTER TABLE ONLY public.dish DROP CONSTRAINT dish_pkey;
ALTER TABLE ONLY public.dish_evaluate DROP CONSTRAINT dish_evaluate_pkey;
ALTER TABLE ONLY public.dish_add DROP CONSTRAINT dish_add_pkey;
ALTER TABLE ONLY public.data_statistics DROP CONSTRAINT data_statistics_pkey;
ALTER TABLE ONLY public.admin_account DROP CONSTRAINT admin_account_pkey;
ALTER TABLE public.vote_record ALTER COLUMN vote_id DROP DEFAULT;
ALTER TABLE public."user" ALTER COLUMN user_id DROP DEFAULT;
ALTER TABLE public.teacher_suggestion ALTER COLUMN tch_sug_id DROP DEFAULT;
ALTER TABLE public.teacher_data_statistics ALTER COLUMN data_id DROP DEFAULT;
ALTER TABLE public.teacher ALTER COLUMN teacher_id DROP DEFAULT;
ALTER TABLE public.school_suggestion_short ALTER COLUMN sch_sug_id DROP DEFAULT;
ALTER TABLE public.school_suggestion_long ALTER COLUMN sch_sug_id DROP DEFAULT;
ALTER TABLE public.platform_settings ALTER COLUMN setting_id DROP DEFAULT;
ALTER TABLE public.notice ALTER COLUMN notice_id DROP DEFAULT;
ALTER TABLE public.entrance_exam ALTER COLUMN question_id DROP DEFAULT;
ALTER TABLE public.dish_evaluate ALTER COLUMN evaluate_id DROP DEFAULT;
ALTER TABLE public.dish_add ALTER COLUMN add_id DROP DEFAULT;
ALTER TABLE public.dish ALTER COLUMN dish_id DROP DEFAULT;
ALTER TABLE public.data_statistics ALTER COLUMN data_id DROP DEFAULT;
ALTER TABLE public.admin_account ALTER COLUMN admin_id DROP DEFAULT;
DROP SEQUENCE public.vote_record_id;
DROP TABLE public.vote_record;
DROP SEQUENCE public.user_id;
DROP TABLE public."user";
DROP SEQUENCE public.teacher_suggestion_id;
DROP TABLE public.teacher_suggestion;
DROP SEQUENCE public.teacher_id;
DROP SEQUENCE public.teacher_data_statistics_id;
DROP TABLE public.teacher_data_statistics;
DROP TABLE public.teacher;
DROP SEQUENCE public.school_suggestion_short_id;
DROP TABLE public.school_suggestion_short;
DROP SEQUENCE public.school_suggestion_long_id;
DROP TABLE public.school_suggestion_long;
DROP SEQUENCE public.platform_settings_id;
DROP TABLE public.platform_settings;
DROP SEQUENCE public.notice_id;
DROP TABLE public.notice;
DROP SEQUENCE public.entrance_exam_id;
DROP TABLE public.entrance_exam;
DROP SEQUENCE public.dish_id;
DROP SEQUENCE public.dish_evaluate_id;
DROP TABLE public.dish_evaluate;
DROP SEQUENCE public.dish_add_id;
DROP TABLE public.dish_add;
DROP TABLE public.dish;
DROP SEQUENCE public.data_statistics_id;
DROP TABLE public.data_statistics;
DROP SEQUENCE public.admin_account_id;
DROP TABLE public.admin_account;
SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: admin_account; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.admin_account (
    admin_id integer NOT NULL,
    username character varying(50) NOT NULL,
    password character varying(255) NOT NULL,
    nickname character varying(20),
    type smallint NOT NULL
);


--
-- Name: COLUMN admin_account.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.admin_account.type IS '0:最高管理员；1:次级管理员；2:建议管理员；3:饭堂管理员；';


--
-- Name: admin_account_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.admin_account_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: admin_account_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.admin_account_id OWNED BY public.admin_account.admin_id;


--
-- Name: data_statistics; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.data_statistics (
    visit integer DEFAULT 0,
    suggestion_num integer DEFAULT 0,
    evaluate_num integer DEFAULT 0,
    week character varying(10),
    date date DEFAULT now() NOT NULL,
    data_id smallint NOT NULL,
    dish_add_num integer DEFAULT 0
);


--
-- Name: data_statistics_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.data_statistics_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    MAXVALUE 14
    CACHE 1
    CYCLE;


--
-- Name: data_statistics_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.data_statistics_id OWNED BY public.data_statistics.data_id;


--
-- Name: dish; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.dish (
    dish_id bigint NOT NULL,
    name character varying(20) NOT NULL,
    description character varying(255),
    avg_score real DEFAULT 0,
    lastest_evaluate character varying(255),
    evaluate_count integer DEFAULT 0,
    date date NOT NULL
);


--
-- Name: dish_add; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.dish_add (
    add_id bigint NOT NULL,
    name character varying(30) NOT NULL,
    description character varying(255),
    ai_score real DEFAULT 0,
    state smallint DEFAULT 0 NOT NULL,
    vote_count integer DEFAULT 0,
    user_id bigint NOT NULL,
    date date DEFAULT now() NOT NULL
);


--
-- Name: COLUMN dish_add.state; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.dish_add.state IS '0:已申请；1:公开投票';


--
-- Name: dish_add_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.dish_add_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dish_add_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.dish_add_id OWNED BY public.dish_add.add_id;


--
-- Name: dish_evaluate; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.dish_evaluate (
    evaluate_id bigint NOT NULL,
    dish_id bigint NOT NULL,
    user_id bigint NOT NULL,
    score smallint NOT NULL,
    evaluate_text character varying(255),
    vote_count integer DEFAULT 0,
    date date DEFAULT now() NOT NULL
);


--
-- Name: dish_evaluate_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.dish_evaluate_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dish_evaluate_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.dish_evaluate_id OWNED BY public.dish_evaluate.evaluate_id;


--
-- Name: dish_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.dish_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: dish_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.dish_id OWNED BY public.dish.dish_id;


--
-- Name: entrance_exam; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.entrance_exam (
    question_id bigint NOT NULL,
    type smallint NOT NULL,
    question text NOT NULL,
    answer character varying(500) NOT NULL
);


--
-- Name: COLUMN entrance_exam.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.entrance_exam.type IS '0:判断题；1:填空题；2:选择题；';


--
-- Name: entrance_exam_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.entrance_exam_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: entrance_exam_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.entrance_exam_id OWNED BY public.entrance_exam.question_id;


--
-- Name: notice; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.notice (
    notice_id bigint NOT NULL,
    show_text character varying(255) NOT NULL,
    detail text,
    publisher character varying(20),
    is_show boolean DEFAULT true NOT NULL,
    date date NOT NULL
);


--
-- Name: notice_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.notice_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: notice_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.notice_id OWNED BY public.notice.notice_id;


--
-- Name: platform_settings; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.platform_settings (
    setting_id integer NOT NULL,
    name character varying(100) NOT NULL,
    value character varying(255)
);


--
-- Name: platform_settings_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.platform_settings_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: platform_settings_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.platform_settings_id OWNED BY public.platform_settings.setting_id;


--
-- Name: school_suggestion_long; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.school_suggestion_long (
    sch_sug_id bigint NOT NULL,
    title character varying(255) NOT NULL,
    content text NOT NULL,
    is_public boolean NOT NULL,
    is_anonymous boolean NOT NULL,
    name character varying(20),
    class character varying(20),
    grade character varying(20),
    ai_score real DEFAULT 0,
    type smallint DEFAULT 3 NOT NULL,
    vote_count integer DEFAULT 0,
    date date DEFAULT now() NOT NULL
);


--
-- Name: COLUMN school_suggestion_long.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.school_suggestion_long.type IS '0:饭堂；1:教学；2:环境；3:其他';


--
-- Name: school_suggestion_long_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.school_suggestion_long_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: school_suggestion_long_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.school_suggestion_long_id OWNED BY public.school_suggestion_long.sch_sug_id;


--
-- Name: school_suggestion_short; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.school_suggestion_short (
    sch_sug_id bigint NOT NULL,
    title character varying(255) NOT NULL,
    content character varying(400) NOT NULL,
    is_public boolean NOT NULL,
    is_anonymous boolean NOT NULL,
    name character varying(20),
    class character varying(20),
    grade character varying(20),
    ai_score real DEFAULT 0,
    type smallint DEFAULT 3 NOT NULL,
    vote_count integer DEFAULT 0,
    date date DEFAULT now() NOT NULL
);


--
-- Name: COLUMN school_suggestion_short.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.school_suggestion_short.type IS '0:饭堂；1:教学；2:环境；3:其他';


--
-- Name: school_suggestion_short_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.school_suggestion_short_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: school_suggestion_short_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.school_suggestion_short_id OWNED BY public.school_suggestion_short.sch_sug_id;


--
-- Name: teacher; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.teacher (
    teacher_id bigint NOT NULL,
    name character varying(20) NOT NULL,
    subject character varying(20) NOT NULL,
    grade character varying(20) NOT NULL,
    class character varying(40) NOT NULL,
    email character varying(80),
    password character varying(255) NOT NULL
);


--
-- Name: teacher_data_statistics; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.teacher_data_statistics (
    data_id bigint NOT NULL,
    visit integer DEFAULT 0,
    teacher_all_suggestion_num integer DEFAULT 0,
    teacher_suggestion_num integer DEFAULT 0,
    teacher_vote_num integer DEFAULT 0,
    teacher_id bigint,
    week character varying(10),
    date date DEFAULT now() NOT NULL
);


--
-- Name: teacher_data_statistics_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.teacher_data_statistics_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


--
-- Name: teacher_data_statistics_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.teacher_data_statistics_id OWNED BY public.teacher_data_statistics.data_id;


--
-- Name: teacher_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.teacher_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: teacher_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.teacher_id OWNED BY public.teacher.teacher_id;


--
-- Name: teacher_suggestion; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.teacher_suggestion (
    tch_sug_id bigint NOT NULL,
    title character varying(255) NOT NULL,
    content text NOT NULL,
    is_public boolean NOT NULL,
    is_anonymous boolean NOT NULL,
    name character varying(20),
    class character varying(20),
    teacher_id bigint NOT NULL,
    vote_count integer DEFAULT 0,
    date date DEFAULT now() NOT NULL
);


--
-- Name: teacher_suggestion_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.teacher_suggestion_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: teacher_suggestion_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.teacher_suggestion_id OWNED BY public.teacher_suggestion.tch_sug_id;


--
-- Name: user; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public."user" (
    user_id bigint NOT NULL,
    unique_identification character varying(255),
    verify_image_true text,
    verify_image_false text,
    fast_key text NOT NULL,
    ip_address character varying(255) DEFAULT false
);


--
-- Name: user_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.user_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: user_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.user_id OWNED BY public."user".user_id;


--
-- Name: vote_record; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.vote_record (
    vote_id bigint NOT NULL,
    user_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type smallint NOT NULL,
    date date DEFAULT now() NOT NULL
);


--
-- Name: COLUMN vote_record.type; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON COLUMN public.vote_record.type IS '0:菜品评价投票；1:加新菜投票；2:教师建议投票；3:对校短建议；4:对校长建议；';


--
-- Name: vote_record_id; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE public.vote_record_id
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- Name: vote_record_id; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE public.vote_record_id OWNED BY public.vote_record.vote_id;


--
-- Name: admin_account admin_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.admin_account ALTER COLUMN admin_id SET DEFAULT nextval('public.admin_account_id'::regclass);


--
-- Name: data_statistics data_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.data_statistics ALTER COLUMN data_id SET DEFAULT nextval('public.data_statistics_id'::regclass);


--
-- Name: dish dish_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish ALTER COLUMN dish_id SET DEFAULT nextval('public.dish_id'::regclass);


--
-- Name: dish_add add_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish_add ALTER COLUMN add_id SET DEFAULT nextval('public.dish_add_id'::regclass);


--
-- Name: dish_evaluate evaluate_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish_evaluate ALTER COLUMN evaluate_id SET DEFAULT nextval('public.dish_evaluate_id'::regclass);


--
-- Name: entrance_exam question_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.entrance_exam ALTER COLUMN question_id SET DEFAULT nextval('public.entrance_exam_id'::regclass);


--
-- Name: notice notice_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.notice ALTER COLUMN notice_id SET DEFAULT nextval('public.notice_id'::regclass);


--
-- Name: platform_settings setting_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.platform_settings ALTER COLUMN setting_id SET DEFAULT nextval('public.platform_settings_id'::regclass);


--
-- Name: school_suggestion_long sch_sug_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.school_suggestion_long ALTER COLUMN sch_sug_id SET DEFAULT nextval('public.school_suggestion_long_id'::regclass);


--
-- Name: school_suggestion_short sch_sug_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.school_suggestion_short ALTER COLUMN sch_sug_id SET DEFAULT nextval('public.school_suggestion_short_id'::regclass);


--
-- Name: teacher teacher_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher ALTER COLUMN teacher_id SET DEFAULT nextval('public.teacher_id'::regclass);


--
-- Name: teacher_data_statistics data_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher_data_statistics ALTER COLUMN data_id SET DEFAULT nextval('public.teacher_data_statistics_id'::regclass);


--
-- Name: teacher_suggestion tch_sug_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher_suggestion ALTER COLUMN tch_sug_id SET DEFAULT nextval('public.teacher_suggestion_id'::regclass);


--
-- Name: user user_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public."user" ALTER COLUMN user_id SET DEFAULT nextval('public.user_id'::regclass);


--
-- Name: vote_record vote_id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.vote_record ALTER COLUMN vote_id SET DEFAULT nextval('public.vote_record_id'::regclass);


--
-- Data for Name: admin_account; Type: TABLE DATA; Schema: public; Owner: -
--

INSERT INTO public.admin_account VALUES (2, 'baseadmin', '$2a$12$nFYMVnWe5luGbygtYWIib.SzYZYv7WcBCLYSWqfIV.0P0oQdjJu/u', '管理员', 1);
INSERT INTO public.admin_account VALUES (3, 'suggestionadmin', '$2a$12$ajxdGMQCeb/31DQGo649..TTPkL4zvRniP2iKaHlchKQ9Am5Q/qHS', '建议管理员', 2);
INSERT INTO public.admin_account VALUES (4, 'canteenadmin', '$2a$12$nI8L86.JLFQYVJWozA/ixuqPxLx9y.0ZDGhYtBdSXF26ezskfO0Yq', '饭堂管理员', 3);
INSERT INTO public.admin_account VALUES (1, 'admin', '$2a$12$IH2MGXGdW1Zu1ibOFXroG.cndyZHCTatseUqAYxcyaxHG0cueUw/m', '最高管理员', 0);


--
-- Data for Name: data_statistics; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: dish; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: dish_add; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: dish_evaluate; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: entrance_exam; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: notice; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: platform_settings; Type: TABLE DATA; Schema: public; Owner: -
--

INSERT INTO public.platform_settings VALUES (7, 'is_show_home_title_image', '1');
INSERT INTO public.platform_settings VALUES (8, 'is_show_canteen_entrance', '1');
INSERT INTO public.platform_settings VALUES (9, 'is_show_dish_add_entrance', '1');
INSERT INTO public.platform_settings VALUES (10, 'ai_api_link', '');
INSERT INTO public.platform_settings VALUES (11, 'ai_api_key', '');
INSERT INTO public.platform_settings VALUES (12, 'ai_model_name', '');
INSERT INTO public.platform_settings VALUES (1, 'school_name', '');
INSERT INTO public.platform_settings VALUES (5, 'web_logo', '');
INSERT INTO public.platform_settings VALUES (6, 'home_title_image', '');
INSERT INTO public.platform_settings VALUES (3, 'icp_num', '');
INSERT INTO public.platform_settings VALUES (4, 'beian_mps', '');
INSERT INTO public.platform_settings VALUES (13, 'teacher_admin_bottom_saying', '');
INSERT INTO public.platform_settings VALUES (2, 'web_title', '');
INSERT INTO public.platform_settings VALUES (17, 'admin_verification_teachers', '');
INSERT INTO public.platform_settings VALUES (16, 'admin_password_to_be_reset', '');
INSERT INTO public.platform_settings VALUES (15, 'admin_reset_verification_code', '');


--
-- Data for Name: school_suggestion_long; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: school_suggestion_short; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: teacher; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: teacher_data_statistics; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: teacher_suggestion; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: user; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Data for Name: vote_record; Type: TABLE DATA; Schema: public; Owner: -
--



--
-- Name: admin_account_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.admin_account_id', 4, true);


--
-- Name: data_statistics_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.data_statistics_id', 1, false);


--
-- Name: dish_add_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.dish_add_id', 1, false);


--
-- Name: dish_evaluate_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.dish_evaluate_id', 1, false);


--
-- Name: dish_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.dish_id', 1, false);


--
-- Name: entrance_exam_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.entrance_exam_id', 1, false);


--
-- Name: notice_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.notice_id', 1, false);


--
-- Name: platform_settings_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.platform_settings_id', 17, true);


--
-- Name: school_suggestion_long_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.school_suggestion_long_id', 1, false);


--
-- Name: school_suggestion_short_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.school_suggestion_short_id', 1, false);


--
-- Name: teacher_data_statistics_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.teacher_data_statistics_id', 1, false);


--
-- Name: teacher_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.teacher_id', 1, false);


--
-- Name: teacher_suggestion_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.teacher_suggestion_id', 1, false);


--
-- Name: user_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.user_id', 1, false);


--
-- Name: vote_record_id; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('public.vote_record_id', 1, false);


--
-- Name: admin_account admin_account_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.admin_account
    ADD CONSTRAINT admin_account_pkey PRIMARY KEY (admin_id);


--
-- Name: data_statistics data_statistics_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.data_statistics
    ADD CONSTRAINT data_statistics_pkey PRIMARY KEY (data_id);


--
-- Name: dish_add dish_add_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish_add
    ADD CONSTRAINT dish_add_pkey PRIMARY KEY (add_id);


--
-- Name: dish_evaluate dish_evaluate_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish_evaluate
    ADD CONSTRAINT dish_evaluate_pkey PRIMARY KEY (evaluate_id);


--
-- Name: dish dish_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.dish
    ADD CONSTRAINT dish_pkey PRIMARY KEY (dish_id);


--
-- Name: entrance_exam entrance_exam_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.entrance_exam
    ADD CONSTRAINT entrance_exam_pkey PRIMARY KEY (question_id);


--
-- Name: notice notice_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.notice
    ADD CONSTRAINT notice_pkey PRIMARY KEY (notice_id);


--
-- Name: platform_settings platform_settings_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.platform_settings
    ADD CONSTRAINT platform_settings_pkey PRIMARY KEY (setting_id);


--
-- Name: school_suggestion_long school_suggestion_short_copy1_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.school_suggestion_long
    ADD CONSTRAINT school_suggestion_short_copy1_pkey PRIMARY KEY (sch_sug_id);


--
-- Name: school_suggestion_short school_suggestion_short_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.school_suggestion_short
    ADD CONSTRAINT school_suggestion_short_pkey PRIMARY KEY (sch_sug_id);


--
-- Name: teacher_data_statistics teacher_data_statistics_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher_data_statistics
    ADD CONSTRAINT teacher_data_statistics_pkey PRIMARY KEY (data_id);


--
-- Name: teacher teacher_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher
    ADD CONSTRAINT teacher_pkey PRIMARY KEY (teacher_id);


--
-- Name: teacher_suggestion teacher_suggestion_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.teacher_suggestion
    ADD CONSTRAINT teacher_suggestion_pkey PRIMARY KEY (tch_sug_id);


--
-- Name: user user_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public."user"
    ADD CONSTRAINT user_pkey PRIMARY KEY (user_id);


--
-- Name: vote_record vote_record_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.vote_record
    ADD CONSTRAINT vote_record_pkey PRIMARY KEY (vote_id);


--
-- PostgreSQL database dump complete
--

\unrestrict sxN143EiG5DehfUmQOAQdDUpzPfGsI1aAtxmLReblfeIIPz4EJJFQLqxIFS10Gj


interface FormItemProps {
  notice_id?: number;
  /** 用于判断是`新增`还是`修改` */
  title: string;
  publisher: string;
  show_text: string;
  detail?: string;
  is_show?: boolean;
  date: string;
}
interface FormProps {
  formInline: FormItemProps;
}

interface RoleFormItemProps {
  publisher: string;
  show_text: string;
  detail?: string;
  is_show?: boolean;
  notice_id?: number;
  date: string;
}
interface RoleFormProps {
  formInline: RoleFormItemProps;
}

export type { FormItemProps, FormProps, RoleFormItemProps, RoleFormProps };
